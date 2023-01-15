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

glm::vec3 Utility::hexToColor(int hex)
{
   float r = static_cast<float>(((hex >> 16) & 0xff)) / 255.0f;
   float g = static_cast<float>(((hex >> 8) & 0xff)) / 255.0f;
   float b = static_cast<float>((hex & 0xff)) / 255.0f;

   return glm::vec3(r, g, b);
}
