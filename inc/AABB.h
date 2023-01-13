#ifndef AABB_H
#define AABB_H

#include "glm/glm.hpp"

#include "Ray.h"

class AABB
{
public:

   AABB();
   AABB(const glm::vec3& min, const glm::vec3& max);

   glm::vec3 getMin() { return mMin; }
   glm::vec3 getMax() { return mMax; }
   glm::vec3 getLength() { return mLength; }

   bool      intersects(const AABB& aabb);
   bool      intersects(const Ray& ray);

   bool      isCompletelyInside(const AABB& aabb);

protected:

   glm::vec3 mMin;
   glm::vec3 mMax;
   glm::vec3 mLength;
};

#endif
