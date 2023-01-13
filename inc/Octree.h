#ifndef OCTREE_H
#define OCTREE_H

#include "OctreeNode.h"

class Octree : public OctreeNode
{
public:

   Octree(const glm::vec3& min, const glm::vec3& max);

   void addTriangles(const std::vector<Triangle>& triangles);
};

#endif
