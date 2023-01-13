#ifndef PLANE_H
#define PLANE_H

#include "glm/glm.hpp"

struct Plane
{
   Plane();
   Plane(const glm::vec3& pointOnPlane, const glm::vec3& normal);

   bool  isFrontFacingTo(const glm::vec3& direction);
   float signedDistanceTo(const glm::vec3& point);

   glm::vec3 pointOnPlane;
   glm::vec3 normal;
};

#endif
