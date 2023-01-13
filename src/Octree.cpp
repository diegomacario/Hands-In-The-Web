#include "Octree.h"

Octree::Octree(const glm::vec3& min, const glm::vec3& max)
   : OctreeNode(min, max)
{

}

void Octree::addTriangles(const std::vector<Triangle>& triangles)
{
   for (const Triangle& triangle : triangles)
   {
      addTriangle(triangle);
   }
}
