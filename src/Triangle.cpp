#include "Triangle.h"

Triangle::Triangle()
   : vertexA(0.0f)
   , vertexB(0.0f)
   , vertexC(0.0f)
   , normal(0.0f)
{

}

Triangle::Triangle(const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& vC)
   : vertexA(vA)
   , vertexB(vB)
   , vertexC(vC)
{
   normal = glm::normalize(glm::cross(vertexB - vertexA, vertexC - vertexA));
}

bool Triangle::isFrontFacingTo(const glm::vec3& direction) const
{
   return (glm::dot(normal, direction) <= 0);
}

float Triangle::signedDistanceTo(const glm::vec3& point) const
{
   /*
      The equation of a plane is the following:

      (P - A) dot N = 0

      Where A is any point on the plane and N is the normal of the plane
      The equation is only satisfied when the vector (P - A) is perpendicular to N, which tells us that P is on the plane
      If we expand it, we obtain this:

      ((X, Y, Z) - (Xo, Yo, Zo)) dot (A, B, C) = 0

      A * (X - Xo) + B * (Y - Yo) + C * (Z - Zo) = 0

      (A * X) + (B * Y) + (C * Z) = (A * Xo) + (B * Yo) + (C * Zo)

      (A * X) + (B * Y) + (C * Z) = D

      Where D = (A * Xo) + (B * Yo) + (C * Zo)

      The distance between a point P and the plane is the length of the projection of (P - A) onto N

      The scalar projection of a vector B onto a vector A is given by:

      (B dot A) / |A| = (|B| * |A| * cos(theta)) / |A| = |B| * cos(theta)

      The vector projection is simply the scalar projection times the unit vector A / |A| (i.e. the component of B along A):

      ((B dot A) / |A|) * (A / |A|)

      In this method we want to calculate the scalar projection of (P - A) onto N, and since N is a unit vector, the scalar projection equation simplifies to:

      ((P - A) dot N) / |N| = (P - A) dot N = (P dot N) - (A dot N)

      As an aside, it just so happens that for a plane, (A dot N) is the plane constant D, so the equation above can also be written like this:

      (P dot N) - D
   */

   return (glm::dot(point, normal) - glm::dot(vertexA, normal));
}

// TODO: Try super optimized version from page 34 of the collision documentation (it is commented out below this method)
bool Triangle::isPointInTriangle(const glm::vec3& point) const
{
   // Check if P is to the left of each edge to determine if it lies inside the triangle
   /*
                C
               / \
              /   \
             /     \
            /   P   \
           /         \
          A-----------B
   */

   // If P is to the left of edge (C - B), the normal of sub-triangle BCP will point in the same direction as the normal of triangle ABC
   // If P is to the right of edge (C - B), the normal of sub-triangle BCP will point in the opposite direction of the normal of triangle ABC
   glm::vec3 normalOfSubTri = glm::cross((vertexC - vertexB), (point - vertexB));
   if (dot(normalOfSubTri, normal) < 0)
   {
      return false; // P is to the right of edge (C - B)
   }

   // If P is to the left of edge (B - A), the normal of sub-triangle ABP will point in the same direction as the normal of triangle ABC
   // If P is to the right of edge (B - A), the normal of sub-triangle ABP will point in the opposite direction of the normal of triangle ABC
   normalOfSubTri = glm::cross((vertexB - vertexA), (point - vertexA));
   if (dot(normalOfSubTri, normal) < 0)
   {
      return false; // P is to the right of edge (B - A)
   }

   // If P is to the left of edge (A - C), the normal of sub-triangle CAP will point in the same direction as the normal of triangle ABC
   // If P is to the right of edge (A - C), the normal of sub-triangle CAP will point in the opposite direction of the normal of triangle ABC
   normalOfSubTri = glm::cross((vertexA - vertexC), (point - vertexC));
   if (dot(normalOfSubTri, normal) < 0)
   {
      return false; // P is to the right of edge (A - C)
   }

   // P lies inside triangle ABC
   return true;
}

/*
typedef unsigned int uint32;
#define in(a) ((uint32&) a)
bool Triangle::isPointInTriangle(const glm::vec3& point)
{
   glm::vec3 e10 = vertexB - vertexA;
   glm::vec3 e20 = vertexC - vertexA;
   float a = glm::dot(e10, e10);
   float b = glm::dot(e10, e20);
   float c = glm::dot(e20, e20);
   float ac_bb = (a * c) - (b * b);
   glm::vec3 vp(point.x - vertexA.x, point.y - vertexA.y, point.z - vertexA.z);
   float d = glm::dot(vp, e10);
   float e = glm::dot(vp, e20);
   float x = (d * c) - (e * b);
   float y = (e * a) - (d * b);
   float z = x + y - ac_bb;
   return ((in(z) & ~(in(x) | in(y))) & 0x80000000);
}
*/

bool Triangle::doesRayIntersectTriangle(const Ray& ray, glm::vec3& outHitPoint) const
{
   /*
      To determine if a ray intersects a triangle, we first check if the ray intersects the plane in which the triangle lies
      If it does, we then check if the hit point lies within the triangle

      The equation of a plane is the following:

      (P - A) dot N = 0

      Where A is any point on the plane and N is the normal of the plane
      The equation is only satisfied when the vector (P - A) is perpendicular to N, which tells us that P is on the plane

      If we expand the equation, replace P with the equation of a ray, and solve for t, we obtain the following:

      (P dot N) - (A dot N) = 0

      (P dot N) = (A dot N)

      ((origin + direction * t) dot N) = (A dot N)

      (origin dot N) + ((direction * t) dot N) = (A dot N)

      ((direction * t) dot N) = (A dot N) - (origin dot N)

      t = ((A dot N) - (origin dot N)) / (direction dot N)

      The equation above gives us the distance along the ray to the point where it intersects the plane
      Notice that if the denominator (direction dot N) is equal to zero, the equation blows up
      That makes sense because the denominator is only equal to zero when the ray and the normal of the plane are perpendicular,
      or in other words, when the ray and the plane are parallel, which means that the ray doesn't intersect the plane

      Once we know the distance along the ray to the point where it intersects the plane, we can calculate the hit point by plugging it into the equation of the ray

      hitPoint = origin + (direction * t)

      And now the question becomes: does the hit point lie inside the triangle? We can answer that question using barycentric coordinates
      Consider this triangle:

            C
           / \
          /   \
         /     \
        /   P   \
       /         \
      A-----------B

      Barycentric coordinates can be used to express the position of any point located in the triangle with three scalars:

      P = (u * A) + (v * B) + (w * C)

      Where 0 <= u, v, w <= 1 and u + v + w = 1

      To derive the rules above, picture the triangle as a 2D coordinate system whose origin is point A and whose axes are vectors (B - A) and (C - A)
      In this coordinate system, any point in the triangle can be expressed as follows:

      P = A + (u * (B - A)) + (v * (C - A))

      Where 0 <= u, v <= 1 and u + v = 1

      If we expand the equation above we obtain the following:

      P = A + (u * B) - (u * A) + (v * C) - (v * A)

      P = ((1 - u - v) * A) + (u * B) + (v * C)

      P = (w * A) + (u * B) + (v * C)

      Where the third barycentric coordinate is given by:

      w = 1 - u - v

      To determine if the hit point lies inside the triangle we can calculate the barycentric coordinates and check that they are valid

      However, the function below uses a more efficient method: it simply checks if the hit point is to the left of the three edges that make up the triangle
   */

   // Calculate the denominator of the solved plane equation:
   // t = ((A dot N) - (origin dot N)) / (direction dot N)
   float rayDirDotNormal = glm::dot(ray.direction, normal);

   // If the ray is perpendicular to the normal of the triangle, the ray and the plane in which the triangle lies are parallel
   // TODO: Use constant here
   // NOTE: This check has been updated to also exclude triangles whose normal points in the same direction as the ray by adding this:
   //       rayDirDotNormal > 0
   //       When that is the case, the ray is hitting the backface of the triangle, which is why we want to ignore it
   if (rayDirDotNormal > 0 || glm::abs(rayDirDotNormal) < 0.0000001f)
   {
      return false;
   }

   // Evaluate the solved plane equation:
   // t = ((A dot N) - (origin dot N)) / (direction dot N)
   float distAlongRayToHit = (glm::dot(vertexA, normal) - glm::dot(ray.origin, normal)) / rayDirDotNormal;

   // If the distance along to the ray to the hit point is negative, the plane in which the triangle lies is behind the origin of the ray
   if (distAlongRayToHit < 0)
   {
      return false;
   }

   // Calculate the hit point
   outHitPoint = (ray.direction * distAlongRayToHit) + ray.origin;

   return isPointInTriangle(outHitPoint);
}
