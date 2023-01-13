#include "AABB.h"

AABB::AABB()
   : mMin(0.0f)
   , mMax(0.0f)
   , mLength(0.0f)
{

}

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
   : mMin(min)
   , mMax(max)
   , mLength(glm::abs(max - min))
{

}

// TODO: Study the algorithm for intersecting AABBs to make sure this is efficient/correct
bool AABB::intersects(const AABB& aabb)
{
   double xt1 = (aabb.mLength.x + mLength.x);
   double xt2 = glm::abs(glm::max(mMax.x, aabb.mMax.x) - glm::min(mMin.x, aabb.mMin.x));

   if (xt2 > xt1)
   {
      return false;
   }

   double yt1 = (aabb.mLength.y + mLength.y);
   double yt2 = glm::abs(glm::max(mMax.y, aabb.mMax.y) - glm::min(mMin.y, aabb.mMin.y));

   if (yt2 > yt1)
   {
      return false;
   }

   double zt1 = (aabb.mLength.z + mLength.z);
   double zt2 = glm::abs(glm::max(mMax.z, aabb.mMax.z) - glm::min(mMin.z, aabb.mMin.z));

   return zt2 <= zt1;
}

// Implementation from part 2 of this article: https://tavianator.com/2011/ray_box.html
// TODO: Study this algorithm to determine if this is the fastest/safest implementation
bool AABB::intersects(const Ray& ray)
{
   double t1 = (mMin[0] - ray.origin[0]) * ray.directionInverse[0];
   double t2 = (mMax[0] - ray.origin[0]) * ray.directionInverse[0];

   double tmin = glm::min(t1, t2);
   double tmax = glm::max(t1, t2);

   for (int i = 1; i < 3; ++i)
   {
      t1 = (mMin[i] - ray.origin[i]) * ray.directionInverse[i];
      t2 = (mMax[i] - ray.origin[i]) * ray.directionInverse[i];

      tmin = glm::max(tmin, glm::min(t1, t2));
      tmax = glm::min(tmax, glm::max(t1, t2));
   }

   // TODO: Using >= here instead of > based on comments in part 2 of the article. Determine if this is correct.
   return tmax >= glm::max(tmin, 0.0);
}

// TODO: Study the algorithm for intersecting AABBs to make sure this is efficient/correct
bool AABB::isCompletelyInside(const AABB& aabb)
{
   return (mMin.x >= aabb.mMin.x) && (mMin.x <= aabb.mMax.x) &&
          (mMax.x >= aabb.mMin.x) && (mMax.x <= aabb.mMax.x) &&
          (mMin.y >= aabb.mMin.y) && (mMin.y <= aabb.mMax.y) &&
          (mMax.y >= aabb.mMin.y) && (mMax.y <= aabb.mMax.y) &&
          (mMin.z >= aabb.mMin.z) && (mMin.z <= aabb.mMax.z) &&
          (mMax.z >= aabb.mMin.z) && (mMax.z <= aabb.mMax.z);
}
