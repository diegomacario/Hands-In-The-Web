#include "OctreeNode.h"

OctreeNode::OctreeNode()
   : mTriangleAABBs()
   , mChildren()
   , mPartitioned(false)
   , AABB()
{

}

OctreeNode::OctreeNode(const glm::vec3& min, const glm::vec3& max)
   : mTriangleAABBs()
   , mChildren()
   , mPartitioned(false)
   , AABB(min, max)
{

}

OctreeNode::~OctreeNode()
{
   for (OctreeNode* node : mChildren)
   {
      delete node;
   }
}

void OctreeNode::partition()
{
   glm::vec3 b1Min = glm::vec3(mMin);
   glm::vec3 b1Max = glm::vec3(mMin.x + mLength.x / 2, mMin.y + mLength.y / 2, mMin.z + mLength.z / 2);
   glm::vec3 b2Min = glm::vec3(mMin.x + mLength.x / 2, mMin.y, mMin.z);
   glm::vec3 b2Max = glm::vec3(mMin.x + mLength.x, mMin.y + mLength.y / 2, mMin.z + mLength.z / 2);
   glm::vec3 b3Min = glm::vec3(mMin.x, mMin.y, mMin.z + mLength.z / 2);
   glm::vec3 b3Max = glm::vec3(mMin.x + mLength.x / 2, mMin.y + mLength.y / 2, mMin.z + mLength.z);
   glm::vec3 b4Min = glm::vec3(mMin.x + mLength.x / 2, mMin.y, mMin.z + mLength.z / 2);
   glm::vec3 b4Max = glm::vec3(mMin.x + mLength.x, mMin.y + mLength.y / 2, mMin.z + mLength.z);
   glm::vec3 b5Min = glm::vec3(mMin.x, mMin.y + mLength.y / 2, mMin.z);
   glm::vec3 b5Max = glm::vec3(mMin.x + mLength.x / 2, mMin.y + mLength.y, mMin.z + mLength.z / 2);
   glm::vec3 b6Min = glm::vec3(mMin.x + mLength.x / 2, mMin.y + mLength.y / 2, mMin.z);
   glm::vec3 b6Max = glm::vec3(mMin.x + mLength.x, mMin.y + mLength.y, mMin.z + mLength.z / 2);
   glm::vec3 b7Min = glm::vec3(mMin.x, mMin.y + mLength.y / 2, mMin.z + mLength.z / 2);
   glm::vec3 b7Max = glm::vec3(mMin.x + mLength.x / 2, mMin.y + mLength.y, mMin.z + mLength.z);
   glm::vec3 b8Min = glm::vec3(mMin.x + mLength.x / 2, mMin.y + mLength.y / 2, mMin.z + mLength.z / 2);
   glm::vec3 b8Max = glm::vec3(mMin.x + mLength.x, mMin.y + mLength.y, mMin.z + mLength.z);

   glm::vec3 minMax[8][8] = { {b1Min, b1Max},
                              {b2Min, b2Max},
                              {b3Min, b3Max},
                              {b4Min, b4Max},
                              {b5Min, b5Max},
                              {b6Min, b6Max},
                              {b7Min, b7Max},
                              {b8Min, b8Max} };

   for (int i = 0; i < 8; i++)
   {
      mChildren[i] = new OctreeNode(minMax[i][0], minMax[i][1]);
   }

   mPartitioned = true;
}

void OctreeNode::addTriangle(const Triangle& triangle)
{
   if (!mPartitioned)
   {
      partition();
   }

   bool addFaceInThisOctreeNode = true;

   TriangleAABB triAABB;
   triAABB.setTriangle(triangle);
   for (OctreeNode* node : mChildren)
   {
      if (triAABB.isCompletelyInside(*node))
      {
         addFaceInThisOctreeNode = false;
         node->addTriangle(triangle);
         break;
      }
   }

   if (addFaceInThisOctreeNode)
   {
      mTriangleAABBs.push_back(TriangleAABB(triangle));
   }
}

void OctreeNode::retrieveTriangles(const AABB& aabb, std::vector<Triangle>& outTriangles)
{
   // TODO: Optimize this
   if (mPartitioned)
   {
      for (OctreeNode* node : mChildren)
      {
         if (node->intersects(aabb))
         {
            node->retrieveTriangles(aabb, outTriangles);
         }
      }
   }

   if (intersects(aabb))
   {
      for (TriangleAABB& triAABB : mTriangleAABBs)
      {
         if (triAABB.intersects(aabb))
         {
            outTriangles.push_back(triAABB.getTriangle());
         }
      }
   }
}

void OctreeNode::retrieveTriangles(const Ray& ray, std::vector<Triangle>& outTriangles)
{
   // TODO: Optimize this
   if (mPartitioned)
   {
      for (OctreeNode* node : mChildren)
      {
         if (node->intersects(ray))
         {
            node->retrieveTriangles(ray, outTriangles);
         }
      }
   }

   if (intersects(ray))
   {
      for (TriangleAABB& triAABB : mTriangleAABBs)
      {
         if (triAABB.intersects(ray))
         {
            outTriangles.push_back(triAABB.getTriangle());
         }
      }
   }
}
