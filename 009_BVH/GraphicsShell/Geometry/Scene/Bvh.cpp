#include "Bvh.h"

#include <algorithm>

using namespace Geometry;



const Bvh * Geometry::Bvh::GetRightNode () const
{
    return rightNode;
}

const Aabb& Geometry::Bvh::GetBoundary () const
{
    return boundary;
}

const Bvh * Geometry::Bvh::GetLeftNode () const
{
    return leftNode;
}






