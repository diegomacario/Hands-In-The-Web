#ifndef RAY_H
#define RAY_H

#include "glm/glm.hpp"

/*
   The Ray class is used to represent rays in 3 dimensions.

   The equation of a ray is:

   Ray = origin + (direction * t)

   Where origin is a point and direction is a vector. The parameter t is used to measure distances along the ray.
*/

struct Ray
{
   Ray();
   Ray(const glm::vec3& ori, const glm::vec3& dir);

   glm::vec3 origin;
   glm::vec3 direction;
   glm::vec3 directionInverse;
};

#endif
