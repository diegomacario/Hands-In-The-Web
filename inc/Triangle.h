#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "glm/glm.hpp"

#include "Ray.h"

/*
         C
        / \
       /   \
      /     \
     /       \
    /         \
   A-----------B
*/

struct Triangle
{
   Triangle();
   Triangle(const glm::vec3& vA, const glm::vec3& vB, const glm::vec3& vC);

   bool  isFrontFacingTo(const glm::vec3& direction) const;
   float signedDistanceTo(const glm::vec3& point) const;

   // Assumes that the point is in the triangle plane
   bool  isPointInTriangle(const glm::vec3& point) const;

   bool  doesRayIntersectTriangle(const Ray& ray, glm::vec3& outHitPoint) const;

   // Vertices must be specified in a CCWISE order
   glm::vec3 vertexA;
   glm::vec3 vertexB;
   glm::vec3 vertexC;

   glm::vec3 normal;
};

#endif
