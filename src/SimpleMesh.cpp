#include "SimpleMesh.h"

void SimpleMesh::GetMinAndMaxDimensions(glm::vec3& outMinDimensions, glm::vec3& outMaxDimensions) const
{
   outMinDimensions = glm::vec3(std::numeric_limits<float>::max());
   outMaxDimensions = glm::vec3(std::numeric_limits<float>::lowest());

   for (const glm::vec3& pos : mPositions)
   {
      if (pos.x < outMinDimensions.x) outMinDimensions.x = pos.x;
      if (pos.y < outMinDimensions.y) outMinDimensions.y = pos.y;
      if (pos.z < outMinDimensions.z) outMinDimensions.z = pos.z;
      if (pos.x > outMaxDimensions.x) outMaxDimensions.x = pos.x;
      if (pos.y > outMaxDimensions.y) outMaxDimensions.y = pos.y;
      if (pos.z > outMaxDimensions.z) outMaxDimensions.z = pos.z;
   }
}
