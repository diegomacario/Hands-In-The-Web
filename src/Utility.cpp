#include "glm/gtx/norm.hpp"

#include "Utility.h"

// This helper function is identical to glm::normalize, except that it checks if the length of the vector
// we want to normalize is zero, and if it is then it doesn't normalize it
glm::vec2 Utility::normalizeWithZeroLengthCheck(const glm::vec2& v)
{
   float squaredLen = glm::length2(v);
   if (squaredLen < Utility::epsilon)
   {
      return v;
   }

   return v / glm::sqrt(squaredLen);
}

// This helper function is identical to glm::normalize, except that it checks if the length of the vector
// we want to normalize is zero, and if it is then it doesn't normalize it
glm::vec3 Utility::normalizeWithZeroLengthCheck(const glm::vec3& v)
{
   float squaredLen = glm::length2(v);
   if (squaredLen < Utility::epsilon)
   {
      return v;
   }

   return v / glm::sqrt(squaredLen);
}
