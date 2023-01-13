#ifndef OCTREE_NODE_H
#define OCTREE_NODE_H

#include <vector>
#include <array>

#include "TriangleAABB.h"

class OctreeNode : public AABB
{
public:

   OctreeNode();
   OctreeNode(const glm::vec3& min, const glm::vec3& max);
   ~OctreeNode();

   void addTriangle(const Triangle& triangle);
   void retrieveTriangles(const AABB& aabb, std::vector<Triangle>& outTriangles);
   void retrieveTriangles(const Ray& ray, std::vector<Triangle>& outTriangles);

private:

   void partition();

   std::vector<TriangleAABB>  mTriangleAABBs;
   std::array<OctreeNode*, 8> mChildren;
   bool                       mPartitioned;
};

#endif
