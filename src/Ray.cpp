#include "Ray.h"

Ray::Ray()
   : origin(0.0f)
   , direction(0.0f)
   , directionInverse(0.0f)
{

}

Ray::Ray(const glm::vec3& ori, const glm::vec3& dir)
   : origin(ori)
   , direction(dir)
   , directionInverse(1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z)
{

}
