#pragma once

#include <vector>

#include "../AABB.h"

namespace Geometry {
    class Bvh {
    public:
        const Bvh* GetLeftNode () const;
        const Bvh* GetRightNode () const;

        const Aabb& GetBoundary () const;
    private:
        friend class Scene;

        Geometry::Aabb boundary;

        Bvh* leftNode  = nullptr;
        Bvh* rightNode = nullptr;

        LinkedAabb faceAabbs[2] = {}; 
    };
};