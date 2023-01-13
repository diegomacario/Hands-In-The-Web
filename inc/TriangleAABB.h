#ifndef TRIANGLE_AABB_H
#define TRIANGLE_AABB_H

#include "AABB.h"
#include "Triangle.h"

class TriangleAABB : public AABB
{
public:

   TriangleAABB();
   TriangleAABB(const Triangle& triangle);

   Triangle getTriangle() { return mTriangle; }
   void     setTriangle(const Triangle& triangle);

private:

   Triangle mTriangle;
};

#endif
