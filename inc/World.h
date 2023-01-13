#ifndef WORLD_H
#define WORLD_H

#include <memory>

#include "SimpleMesh.h"
#include "Octree.h"

class CollisionData
{
public:
   // Ellipsoid radii
   glm::vec3 eRadius;

   // Information about the move being requested in R3
   glm::vec3 positionInR3;
   glm::vec3 translationInR3;

   // Information about the move being requested in ellipsoid space
   glm::vec3 basePoint;
   glm::vec3 translation;
   glm::vec3 normalizedTranslation;

   // Hit information
   bool      foundCollision;
   float     nearestDistance;
   glm::vec3 intersectionPoint;
};

class World
{
public:

   World(std::vector<SimpleMesh>& meshes);

   void checkCollision(CollisionData& collisionData);
   void checkTriangle(CollisionData& collisionData, const glm::vec3& vertexA, const glm::vec3& vertexB, const glm::vec3& vertexC);

   bool checkIntersection(const Ray& ray, glm::vec3& outHitPoint);
   bool checkIntersection(const Ray& ray, glm::vec3& outHitPoint, glm::vec3& outNormal);

private:

   std::vector<Triangle> getTrianglesFromMeshes(std::vector<SimpleMesh>& meshes);
   void                  getMinAndMaxDimensionsFromMeshes(const std::vector<SimpleMesh>& meshes, glm::vec3& outMinDimensions, glm::vec3& outMaxDimensions);
   bool                  getLowestRoot(float a, float b, float c, float maxRoot, float& outRoot);

   std::vector<Triangle>   mTrianglesInR3;
   std::unique_ptr<Octree> mOctree;
};

#endif
