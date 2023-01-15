#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif

#include "AlembicMesh.h"

AlembicMesh::AlembicMesh()
{
   glGenVertexArrays(1, &mVAO);
   glGenBuffers(2, &mVBOs[0]);
   glGenBuffers(1, &mEBO);
}

AlembicMesh::~AlembicMesh()
{
   glDeleteVertexArrays(1, &mVAO);
   glDeleteBuffers(2, &mVBOs[0]);
   glDeleteBuffers(1, &mEBO);
}

AlembicMesh::AlembicMesh(AlembicMesh&& rhs) noexcept
   : mNumVertices(std::exchange(rhs.mNumVertices, 0))
   , mNumIndices(std::exchange(rhs.mNumIndices, 0))
   , mVAO(std::exchange(rhs.mVAO, 0))
   , mVBOs(std::exchange(rhs.mVBOs, std::array<unsigned int, 2>()))
   , mEBO(std::exchange(rhs.mEBO, 0))
{

}

AlembicMesh& AlembicMesh::operator=(AlembicMesh&& rhs) noexcept
{
   mNumVertices = std::exchange(rhs.mNumVertices, 0);
   mNumIndices  = std::exchange(rhs.mNumIndices, 0);
   mVAO         = std::exchange(rhs.mVAO, 0);
   mVBOs        = std::exchange(rhs.mVBOs, std::array<unsigned int, 2>());
   mEBO         = std::exchange(rhs.mEBO, 0);
   return *this;
}

void AlembicMesh::InitializeBuffers(wabc::IMesh* mesh)
{
   mNumVertices = static_cast<unsigned int>(mesh->getPoints().size());
   mNumIndices  = static_cast<unsigned int>(mesh->getFaceIndices().size());

   glBindVertexArray(mVAO);

   // Load the mesh's data into the buffers

   // Positions
   glBindBuffer(GL_ARRAY_BUFFER, mVBOs[VBOTypes::positions]);
   glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(wabc::float3), nullptr, GL_STREAM_DRAW);
   // Normals
   glBindBuffer(GL_ARRAY_BUFFER, mVBOs[VBOTypes::normals]);
   glBufferData(GL_ARRAY_BUFFER, mNumVertices * sizeof(wabc::float3), nullptr, GL_STREAM_DRAW);

   glBindBuffer(GL_ARRAY_BUFFER, 0);

   // Indices
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices * sizeof(int), mesh->getFaceIndices().data(), GL_STATIC_DRAW);

   // Unbind the VAO first, then the EBO
   glBindVertexArray(0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void AlembicMesh::UpdateBuffers(wabc::IMesh* mesh)
{
   wabc::span<wabc::float3> points = mesh->getPoints();
   wabc::span<int> indices = mesh->getFaceIndices();

   std::vector<wabc::float3> normals;
   normals.resize(points.size());
   for (unsigned int i = 0; i < indices.size(); i += 3)
   {
      wabc::float3 v0 = points[indices[i]];
      wabc::float3 v1 = points[indices[i + 1]];
      wabc::float3 v2 = points[indices[i + 2]];

      wabc::float3 v20 = wabc::normalize(v2 - v0);
      wabc::float3 v10 = wabc::normalize(v1 - v0);

      wabc::float3 normal = wabc::normalize(wabc::cross(v20, v10));

      normals[indices[i]] = normal;
      normals[indices[i + 1]] = normal;
      normals[indices[i + 2]] = normal;
   }

   glBindVertexArray(mVAO);

   // Load the mesh's data into the buffers

   // Positions
   glBindBuffer(GL_ARRAY_BUFFER, mVBOs[VBOTypes::positions]);
   glBufferSubData(GL_ARRAY_BUFFER, 0, mNumVertices * sizeof(wabc::float3), mesh->getPoints().data());
   // Normals
   glBindBuffer(GL_ARRAY_BUFFER, mVBOs[VBOTypes::normals]);
   glBufferSubData(GL_ARRAY_BUFFER, 0, mNumVertices * sizeof(wabc::float3), &normals[0]);

   glBindBuffer(GL_ARRAY_BUFFER, 0);

   glBindVertexArray(0);
}

void AlembicMesh::ConfigureVAO(int posAttribLocation,
                               int normalAttribLocation)
{
   glBindVertexArray(mVAO);

   // Set the vertex attribute pointers
   BindFloatAttribute(posAttribLocation,    mVBOs[VBOTypes::positions], 3);
   BindFloatAttribute(normalAttribLocation, mVBOs[VBOTypes::normals], 3);

   glBindVertexArray(0);
}

void AlembicMesh::UnconfigureVAO(int posAttribLocation,
                                 int normalAttribLocation)
{
   glBindVertexArray(mVAO);

   // Unset the vertex attribute pointers
   UnbindAttribute(posAttribLocation,    mVBOs[VBOTypes::positions]);
   UnbindAttribute(normalAttribLocation, mVBOs[VBOTypes::normals]);

   glBindVertexArray(0);
}

void AlembicMesh::BindFloatAttribute(int attribLocation, unsigned int VBO, int numComponents)
{
   if (attribLocation >= 0)
   {
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glEnableVertexAttribArray(attribLocation);
      glVertexAttribPointer(attribLocation, numComponents, GL_FLOAT, GL_FALSE, 0, (void*)0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
   }
}

void AlembicMesh::BindIntAttribute(int attribLocation, unsigned int VBO, int numComponents)
{
   if (attribLocation >= 0)
   {
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glEnableVertexAttribArray(attribLocation);
      glVertexAttribIPointer(attribLocation, numComponents, GL_INT, 0, (void*)0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
   }
}

void AlembicMesh::UnbindAttribute(int attribLocation, unsigned int VBO)
{
   if (attribLocation >= 0)
   {
      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glDisableVertexAttribArray(attribLocation);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
   }
}

// TODO: GL_TRIANGLES shouldn't be hardcoded here
//       Can we load that from the GLTF file?
void AlembicMesh::Render()
{
   glBindVertexArray(mVAO);

   if (mNumIndices > 0)
   {
      glDrawElements(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0);
   }
   else
   {
      glDrawArrays(GL_TRIANGLES, 0, mNumVertices);
   }

   glBindVertexArray(0);
}

// TODO: GL_TRIANGLES shouldn't be hardcoded here
//       Can we load that from the GLTF file?
void AlembicMesh::RenderInstanced(unsigned int numInstances)
{
   glBindVertexArray(mVAO);

   if (mNumIndices > 0)
   {
      glDrawElementsInstanced(GL_TRIANGLES, mNumIndices, GL_UNSIGNED_INT, 0, numInstances);
   }
   else
   {
      glDrawArraysInstanced(GL_TRIANGLES, 0, mNumVertices, numInstances);
   }

   glBindVertexArray(0);
}
