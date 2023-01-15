#ifndef UTILITY_H
#define UTILITY_H

#include "glm/glm.hpp"

namespace Utility
{
   const float epsilon = 0.000001f;

   glm::vec2 normalizeWithZeroLengthCheck(const glm::vec2& v);
   glm::vec3 normalizeWithZeroLengthCheck(const glm::vec3& v);

   glm::vec3 hexToColor(int hex);
}

#endif
