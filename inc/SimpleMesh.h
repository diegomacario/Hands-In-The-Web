#ifndef SIMPLE_MESH_H
#define SIMPLE_MESH_H

#include <vector>

#include "glm/glm.hpp"

class SimpleMesh
{
public:

   SimpleMesh() = default;
   ~SimpleMesh() = default;

   SimpleMesh(const SimpleMesh&) = default;
   SimpleMesh& operator=(const SimpleMesh&) = default;

   SimpleMesh(SimpleMesh&& rhs) = default;
   SimpleMesh& operator=(SimpleMesh&& rhs) = default;

   std::vector<glm::vec3>&    GetPositions() { return mPositions; }
   std::vector<glm::vec3>&    GetNormals()   { return mNormals;   }
   std::vector<glm::vec2>&    GetTexCoords() { return mTexCoords; }
   std::vector<unsigned int>& GetIndices()   { return mIndices;   }
   void                       GetMinAndMaxDimensions(glm::vec3& outMinDimensions, glm::vec3& outMaxDimensions) const;

private:

   std::vector<glm::vec3>    mPositions;
   std::vector<glm::vec3>    mNormals;
   std::vector<glm::vec2>    mTexCoords;
   std::vector<unsigned int> mIndices;
};

#endif
