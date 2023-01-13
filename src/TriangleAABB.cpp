#include "TriangleAABB.h"

TriangleAABB::TriangleAABB()
   : mTriangle()
   , AABB()
{

}

TriangleAABB::TriangleAABB(const Triangle& triangle)
{
   setTriangle(triangle);
}

void TriangleAABB::setTriangle(const Triangle& triangle)
{
   mTriangle = triangle;

   float minX = std::numeric_limits<float>::max();
   float minY = std::numeric_limits<float>::max();
   float minZ = std::numeric_limits<float>::max();
   float maxX = std::numeric_limits<float>::lowest();
   float maxY = std::numeric_limits<float>::lowest();
   float maxZ = std::numeric_limits<float>::lowest();

   // TODO: Replace the blocks of if-statements below with 3 calls to a function
   if (mTriangle.vertexA.x < minX) minX = mTriangle.vertexA.x;
   if (mTriangle.vertexA.y < minY) minY = mTriangle.vertexA.y;
   if (mTriangle.vertexA.z < minZ) minZ = mTriangle.vertexA.z;

   if (mTriangle.vertexA.x > maxX) maxX = mTriangle.vertexA.x;
   if (mTriangle.vertexA.y > maxY) maxY = mTriangle.vertexA.y;
   if (mTriangle.vertexA.z > maxZ) maxZ = mTriangle.vertexA.z;

   if (mTriangle.vertexB.x < minX) minX = mTriangle.vertexB.x;
   if (mTriangle.vertexB.y < minY) minY = mTriangle.vertexB.y;
   if (mTriangle.vertexB.z < minZ) minZ = mTriangle.vertexB.z;

   if (mTriangle.vertexB.x > maxX) maxX = mTriangle.vertexB.x;
   if (mTriangle.vertexB.y > maxY) maxY = mTriangle.vertexB.y;
   if (mTriangle.vertexB.z > maxZ) maxZ = mTriangle.vertexB.z;

   if (mTriangle.vertexC.x < minX) minX = mTriangle.vertexC.x;
   if (mTriangle.vertexC.y < minY) minY = mTriangle.vertexC.y;
   if (mTriangle.vertexC.z < minZ) minZ = mTriangle.vertexC.z;

   if (mTriangle.vertexC.x > maxX) maxX = mTriangle.vertexC.x;
   if (mTriangle.vertexC.y > maxY) maxY = mTriangle.vertexC.y;
   if (mTriangle.vertexC.z > maxZ) maxZ = mTriangle.vertexC.z;

   mMin    = glm::vec3(minX, minY, minZ);
   mMax    = glm::vec3(maxX, maxY, maxZ);
   mLength = glm::abs(mMax - mMin);
}
