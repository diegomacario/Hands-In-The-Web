#include "glm/gtx/norm.hpp"

#include "World.h"
#include "Utility.h"

World::World(std::vector<SimpleMesh>& meshes)
{
   mTrianglesInR3 = getTrianglesFromMeshes(meshes);

   glm::vec3 minDimensions, maxDimensions;
   getMinAndMaxDimensionsFromMeshes(meshes, minDimensions, maxDimensions);

   mOctree = std::make_unique<Octree>(minDimensions, maxDimensions);
   mOctree->addTriangles(mTrianglesInR3);
}

void World::checkCollision(CollisionData& collisionData)
{
   // Create the player's AABB
   // Without the padding value below, the player's AABB is too tight,
   // which causes it to not intersect the AABBs of triangles that are adjacent to the one it's currently intersecting
   // That results is problems like getting stuck on edges when sliding along walls,
   // so it's important that the player's AABB is a little bigger than the player's mesh
   // The padding takes care of that
   float aabbPadding = 1.0f;

   glm::vec3 min(collisionData.positionInR3);
   min.x -= collisionData.eRadius.x;
   min.y -= collisionData.eRadius.y;
   min.z -= collisionData.eRadius.z;
   min   -= aabbPadding;

   glm::vec3 max(collisionData.positionInR3);
   max.x += collisionData.eRadius.x;
   max.y += collisionData.eRadius.y;
   max.z += collisionData.eRadius.z;
   max   += aabbPadding;

   AABB playerAABB(min, max);

   // Find the triangles that the player's ellipsoid might intersect
   std::vector<Triangle> triangles;
   mOctree->retrieveTriangles(playerAABB, triangles);

   // Check those triangles for collisions
   for (const Triangle& triangle : triangles)
   {
      checkTriangle(collisionData,
                    triangle.vertexA / collisionData.eRadius,
                    triangle.vertexB / collisionData.eRadius,
                    triangle.vertexC / collisionData.eRadius);
   }
}

// Assumes vertexA, vertexB and vertexC are given in ellisoid space
void World::checkTriangle(CollisionData& collisionData, const glm::vec3& vertexA, const glm::vec3& vertexB, const glm::vec3& vertexC)
{
   Triangle triangle(vertexA, vertexB, vertexC);

   // Is the triangle front-facing to the translation vector?
   // We only check front-facing triangles
   if (triangle.isFrontFacingTo(collisionData.normalizedTranslation))
   {
      // Interval of plane intersection
      float t0, t1;
      bool embeddedInPlane = false;
      // Calculate the signed distance from sphere position to triangle plane
      float signedDistToTrianglePlane = triangle.signedDistanceTo(collisionData.basePoint);
      // Cache this as we are going to use it a few times below
      float normalDotTranslation = glm::dot(triangle.normal, collisionData.translation);

      // If sphere is travelling parallel to the plane
      if (glm::abs(normalDotTranslation) < Utility::epsilon)
      {
         if (glm::abs(signedDistToTrianglePlane) >= 1.0f)
         {
            // Sphere is not embedded in plane
            // No collision possible
            return;
         }
         else
         {
            // Sphere is embedded in plane
            // It intersects in the whole range [0..1]
            embeddedInPlane = true;
            t0 = 0.0f;
            t1 = 1.0f;
         }
      }
      else // If sphere is not travelling parallel to the plane
      {
         // Calculate the intersection interval
         // t0 is the exact time when the signed distance between the center of the unit sphere and the triangle plane is equal to 1
         // In other words, it's the exact time when the unit sphere rests on the front side of the triangle plane
         // t1 is the opposite: the exact time when the unit sphere rests on the back side of the triangle plane
         // See page 12 of the collision documentation to understand how t0 and t1 are calculated
         t0 = (-1.0f - signedDistToTrianglePlane) / normalDotTranslation;
         t1 = (1.0f - signedDistToTrianglePlane) / normalDotTranslation;

         // Swap so t0 < t1
         if (t0 > t1)
         {
            float temp = t1;
            t1 = t0;
            t0 = temp;
         }

         // Check that at least one result is within range
         if (t0 > 1.0f || t1 < 0.0f)
         {
            // Both t values are outside values [0,1]
            // No collision possible
            return;
         }

         // Clamp to [0,1]
         if (t0 < 0.0f) t0 = 0.0f;
         if (t1 < 0.0f) t1 = 0.0f;
         if (t0 > 1.0f) t0 = 1.0f;
         if (t1 > 1.0f) t1 = 1.0f;
      }

      // At this point we have two time values t0 and t1 between which the swept sphere intersects the triangle plane
      // If any collision is to occur it must happen within this interval
      glm::vec3 collisionPoint;
      bool foundCollison = false;
      float t = 1.0f;

      // First we check for the easy case: a collision with the inside of the triangle
      // If this happens it must be at time t0 as this is when the sphere rests on the front side of the triangle plane
      // Note: this can only happen if the sphere is not embedded in the triangle plane
      if (!embeddedInPlane)
      {
         // See page 13 of the collision documentation to understand how planeIntersectionPoint is calculated
         glm::vec3 planeIntersectionPoint = (collisionData.basePoint - triangle.normal) + (t0 * collisionData.translation);
         if (triangle.isPointInTriangle(planeIntersectionPoint))
         {
            foundCollison = true;
            t = t0;
            collisionPoint = planeIntersectionPoint;
         }
      }

      // If we haven't found a collision already we'll have to sweep-test the sphere against the vertices and edges of the triangle
      // Note: A collision with the inside the triangle will always happen before a vertex or edge collision
      // That is why we can skip the swept test if the previous check finds a collision
      if (foundCollison == false)
      {
         glm::vec3 translation = collisionData.translation;
         glm::vec3 base = collisionData.basePoint;
         float translationSquaredLength = glm::length2(translation);
         float a, b, c;
         float newT;

         // For each vertex or edge a quadratic equation has to be solved. We parameterize this equation as:
         // a*t^2 + b*t + c = 0
         // And below we calculate the parameters a, b and c for each test

         // Check for collisions with the vertices
         // See pages 14-15 of the collision documentation to understand the equations below

         // vertexA
         a = translationSquaredLength;
         b = 2.0f * glm::dot(translation, base - vertexA);
         c = glm::length2(vertexA - base) - 1.0f;
         if (getLowestRoot(a, b, c, t, newT))
         {
            t = newT;
            foundCollison = true;
            collisionPoint = vertexA;
         }

         // vertexB
         b = 2.0f * glm::dot(translation, base - vertexB);
         c = glm::length2(vertexB - base) - 1.0f;
         if (getLowestRoot(a, b, c, t, newT))
         {
            t = newT;
            foundCollison = true;
            collisionPoint = vertexB;
         }

         // vertexC
         b = 2.0f * glm::dot(translation, base - vertexC);
         c = glm::length2(vertexC - base) - 1.0f;
         if (getLowestRoot(a, b, c, t, newT))
         {
            t = newT;
            foundCollison = true;
            collisionPoint = vertexC;
         }

         // Check for collisions with the edges
         // See page 16 of the collision documentation to understand the equations below

         // vertexA -> vertexB
         glm::vec3 edge = vertexB - vertexA;
         glm::vec3 baseToVertex = vertexA - base;
         float edgeSquaredLength = glm::length2(edge);
         float edgeDotTranslation = glm::dot(edge, translation);
         float edgeDotBaseToVertex = glm::dot(edge, baseToVertex);
         // Calculate parameters for equation
         a = (edgeSquaredLength * -translationSquaredLength) + (edgeDotTranslation * edgeDotTranslation);
         b = (edgeSquaredLength * (2.0f * glm::dot(translation, baseToVertex))) - (2.0f * edgeDotTranslation * edgeDotBaseToVertex);
         c = edgeSquaredLength * (1.0f - glm::length2(baseToVertex)) + (edgeDotBaseToVertex * edgeDotBaseToVertex);
         // Does the swept sphere collide against infinite edge?
         if (getLowestRoot(a, b, c, t, newT))
         {
            // Check if intersection is within line segment:
            float f = ((edgeDotTranslation * newT) - edgeDotBaseToVertex) / edgeSquaredLength;
            if (f >= 0.0f && f <= 1.0f)
            {
               // intersection took place within segment.
               t = newT;
               foundCollison = true;
               collisionPoint = vertexA + (f * edge);
            }
         }

         // vertexB -> vertexC
         edge = vertexC - vertexB;
         baseToVertex = vertexB - base;
         edgeSquaredLength = glm::length2(edge);
         edgeDotTranslation = glm::dot(edge, translation);
         edgeDotBaseToVertex = glm::dot(edge, baseToVertex);
         a = (edgeSquaredLength * -translationSquaredLength) + (edgeDotTranslation * edgeDotTranslation);
         b = (edgeSquaredLength * (2.0f * glm::dot(translation, baseToVertex))) - (2.0f * edgeDotTranslation * edgeDotBaseToVertex);
         c = (edgeSquaredLength * (1.0f - glm::length2(baseToVertex))) + (edgeDotBaseToVertex * edgeDotBaseToVertex);
         if (getLowestRoot(a, b, c, t, newT))
         {
            float f = ((edgeDotTranslation * newT) - edgeDotBaseToVertex) / edgeSquaredLength;
            if (f >= 0.0f && f <= 1.0f)
            {
               t = newT;
               foundCollison = true;
               collisionPoint = vertexB + (f * edge);
            }
         }

         // vertexC -> vertexA
         edge = vertexA - vertexC;
         baseToVertex = vertexC - base;
         edgeSquaredLength = glm::length2(edge);
         edgeDotTranslation = glm::dot(edge, translation);
         edgeDotBaseToVertex = glm::dot(edge, baseToVertex);
         a = (edgeSquaredLength * -translationSquaredLength) + (edgeDotTranslation * edgeDotTranslation);
         b = (edgeSquaredLength * (2.0f * glm::dot(translation, baseToVertex))) - (2.0f * edgeDotTranslation * edgeDotBaseToVertex);
         c = (edgeSquaredLength * (1.0f - glm::length2(baseToVertex))) + (edgeDotBaseToVertex * edgeDotBaseToVertex);
         if (getLowestRoot(a, b, c, t, newT))
         {
            float f = ((edgeDotTranslation * newT) - edgeDotBaseToVertex) / edgeSquaredLength;
            if (f >= 0.0f && f <= 1.0f)
            {
               t = newT;
               foundCollison = true;
               collisionPoint = vertexC + (f * edge);
            }
         }
      }

      // Finally, we output the results
      if (foundCollison == true)
      {
         // Distance to collision - t is time of collision
         float distToCollision = t * glm::length(collisionData.translation);

         // Does this triangle qualify for the closest hit?
         // It does if it's the first hit or the closest
         if (collisionData.foundCollision == false || distToCollision < collisionData.nearestDistance)
         {
            // Collision information nessesary for sliding
            collisionData.nearestDistance = distToCollision;
            collisionData.intersectionPoint = collisionPoint;
            collisionData.foundCollision = true;
         }
      }
   }
}

bool World::checkIntersection(const Ray& ray, glm::vec3& outHitPoint)
{
   // Find the triangles that the ray might intersect
   std::vector<Triangle> triangles;
   mOctree->retrieveTriangles(ray, triangles);

   // Check those triangles for intersections
   glm::vec3 hitPoint(0.0f);
   float     squaredDistanceToHitPoint = 0.0f;
   glm::vec3 nearestHitPoint(0.0f);
   float     squaredDistanceToNearestHitPoint = std::numeric_limits<float>::max();
   bool      foundIntersection = false;

   for (const Triangle& triangle : triangles)
   {
      if (triangle.doesRayIntersectTriangle(ray, hitPoint))
      {
         squaredDistanceToHitPoint = glm::length2(hitPoint - ray.origin);
         if (squaredDistanceToHitPoint < squaredDistanceToNearestHitPoint)
         {
            nearestHitPoint = hitPoint;
            squaredDistanceToNearestHitPoint = squaredDistanceToHitPoint;
         }
         foundIntersection = true;
      }
   }

   outHitPoint = nearestHitPoint;
   return foundIntersection;
}

bool World::checkIntersection(const Ray& ray, glm::vec3& outHitPoint, glm::vec3& outNormal)
{
   // Find the triangles that the ray might intersect
   std::vector<Triangle> triangles;
   mOctree->retrieveTriangles(ray, triangles);

   // Check those triangles for intersections
   glm::vec3 hitPoint(0.0f);
   float     squaredDistanceToHitPoint = 0.0f;
   float     squaredDistanceToNearestHitPoint = std::numeric_limits<float>::max();
   bool      foundIntersection = false;

   for (const Triangle& triangle : triangles)
   {
      if (triangle.doesRayIntersectTriangle(ray, hitPoint))
      {
         squaredDistanceToHitPoint = glm::length2(hitPoint - ray.origin);
         if (squaredDistanceToHitPoint < squaredDistanceToNearestHitPoint)
         {
            outHitPoint = hitPoint;
            outNormal   = triangle.normal;
            squaredDistanceToNearestHitPoint = squaredDistanceToHitPoint;
         }

         foundIntersection = true;
      }
   }

   return foundIntersection;
}

// TODO: Resize array of triangles and emplace_back every Triangle
std::vector<Triangle> World::getTrianglesFromMeshes(std::vector<SimpleMesh>& meshes)
{
   std::vector<Triangle> triangles;

   // Loop over the meshes
   unsigned int numMeshes = static_cast<unsigned int>(meshes.size());
   for (unsigned int meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
   {
      SimpleMesh&               mesh      = meshes[meshIndex];
      std::vector<glm::vec3>    positions = mesh.GetPositions();
      std::vector<unsigned int> indices   = mesh.GetIndices();

      if (indices.size() == 0)
      {
         // If the current mesh doesn't have indices, each sequential set of 3 positions makes up a triangle
         unsigned int numPositions = static_cast<unsigned int>(positions.size());
         for (unsigned int i = 0; i < numPositions; i += 3)
         {
            triangles.push_back(Triangle(positions[i + 0],
                                         positions[i + 1],
                                         positions[i + 2]));
         }
      }
      else
      {
         // If the current mesh has indices, each sequential set of 3 indices makes up a triangle
         unsigned int numIndices = static_cast<unsigned int>(indices.size());
         for (unsigned int i = 0; i < numIndices; i += 3)
         {
            triangles.push_back(Triangle(positions[indices[i + 0]],
                                         positions[indices[i + 1]],
                                         positions[indices[i + 2]]));
         }
      }
   }

   return triangles;
}

void World::getMinAndMaxDimensionsFromMeshes(const std::vector<SimpleMesh>& meshes, glm::vec3& outMinDimensions, glm::vec3& outMaxDimensions)
{
   glm::vec3 minDimensionsOfCurrMesh, maxDimensionsOfCurrMesh;
   std::vector<glm::vec3> minDimensionsOfMeshes, maxDimensionsOfMeshes;
   for (const SimpleMesh& mesh : meshes)
   {
      mesh.GetMinAndMaxDimensions(minDimensionsOfCurrMesh, maxDimensionsOfCurrMesh);
      minDimensionsOfMeshes.push_back(minDimensionsOfCurrMesh);
      maxDimensionsOfMeshes.push_back(maxDimensionsOfCurrMesh);
   }

   outMinDimensions = glm::vec3(std::numeric_limits<float>::max());
   for (const glm::vec3& minDimensions : minDimensionsOfMeshes)
   {
      if (minDimensions.x < outMinDimensions.x) outMinDimensions.x = minDimensions.x;
      if (minDimensions.y < outMinDimensions.y) outMinDimensions.y = minDimensions.y;
      if (minDimensions.z < outMinDimensions.z) outMinDimensions.z = minDimensions.z;
   }

   outMaxDimensions = glm::vec3(std::numeric_limits<float>::lowest());
   for (const glm::vec3& maxDimensions : maxDimensionsOfMeshes)
   {
      if (maxDimensions.x > outMaxDimensions.x) outMaxDimensions.x = maxDimensions.x;
      if (maxDimensions.y > outMaxDimensions.y) outMaxDimensions.y = maxDimensions.y;
      if (maxDimensions.z > outMaxDimensions.z) outMaxDimensions.z = maxDimensions.z;
   }
}

bool World::getLowestRoot(float a, float b, float c, float maxRoot, float& outRoot)
{
   float determinant = (b * b) - (4.0f * a * c);

   // If the determinant is negative, it means no solutions
   if (determinant < 0.0f)
   {
      return false;
   }

   // Calculate the two roots
   // If the determinant is equal to 0 then root1 is equal to root2, but let's disregard that slight optimization
   float squareRootOfDeterminant = glm::sqrt(determinant);
   float root1 = (-b - squareRootOfDeterminant) / (2 * a);
   float root2 = (-b + squareRootOfDeterminant) / (2 * a);

   // Swap so root1 <= root2
   if (root1 > root2)
   {
      float temp = root2;
      root2 = root1;
      root1 = temp;
   }

   // Get the lowest valid root
   if (root1 > 0 && root1 < maxRoot)
   {
      outRoot = root1;
      return true;
   }

   // It is possible that we want r2
   // This can happen if r1 < 0
   if (root2 > 0 && root2 < maxRoot)
   {
      outRoot = root2;
      return true;
   }

   // No valid roots
   return false;
}
