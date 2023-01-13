#ifndef ALEMBIC_MESH_H
#define ALEMBIC_MESH_H

#include <vector>
#include <array>

#include "glm/glm.hpp"

#include "WebAlembicViewer.h"

class AlembicMesh
{
public:

   AlembicMesh();
   ~AlembicMesh();

   AlembicMesh(const AlembicMesh&) = delete;
   AlembicMesh& operator=(const AlembicMesh&) = delete;

   AlembicMesh(AlembicMesh&& rhs) noexcept;
   AlembicMesh& operator=(AlembicMesh&& rhs) noexcept;

   void                       InitializeBuffers(wabc::IMesh* mesh);
   void                       UpdateBuffers(wabc::IMesh* mesh);

   void                       ConfigureVAO(int posAttribLocation,
                                           int normalAttribLocation);

   void                       UnconfigureVAO(int posAttribLocation,
                                             int normalAttribLocation);

   void                       BindFloatAttribute(int attribLocation, unsigned int VBO, int numComponents);
   void                       BindIntAttribute(int attribLocation, unsigned int VBO, int numComponents);
   void                       UnbindAttribute(int attribLocation, unsigned int VBO);

   void                       Render();
   void                       RenderInstanced(unsigned int numInstances);

private:

   enum VBOTypes : unsigned int
   {
      positions  = 0,
      normals    = 1,
   };

   unsigned int                mNumVertices;
   unsigned int                mNumIndices;
   unsigned int                mVAO;
   std::array<unsigned int, 2> mVBOs;
   unsigned int                mEBO;
};

#endif
