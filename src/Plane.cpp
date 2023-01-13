#include "Plane.h"

Plane::Plane()
   : pointOnPlane(0.0f)
   , normal(0.0f)
{

}

Plane::Plane(const glm::vec3& pointOnPlane, const glm::vec3& normal)
   : pointOnPlane(pointOnPlane)
   , normal(normal)
{

}

bool Plane::isFrontFacingTo(const glm::vec3& direction)
{
   return (glm::dot(normal, direction) <= 0);
}

float Plane::signedDistanceTo(const glm::vec3& point)
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

   return (glm::dot(point, normal) - glm::dot(pointOnPlane, normal));
}
