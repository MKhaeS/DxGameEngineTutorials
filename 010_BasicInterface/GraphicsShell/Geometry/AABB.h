#pragma once


#include "Vector.h"

namespace Geometry {
    struct Aabb {
        Vector3 Min = { 0.0f, 0.0f, 0.0f };
        Vector3 Max = { 0.0f, 0.0f, 0.0f };
    };

    struct LinkedAabb : public Aabb {
        union {
            int faceIdx = 0;
            int meshIdx;
        };
    };

    struct AxisSortedAabb : public LinkedAabb {
        int posInMinArray[3] = {};
        int idxInMaxArray[3] = {};

        AxisSortedAabb () = default;

        AxisSortedAabb (const LinkedAabb& face_aabb) :
            LinkedAabb (face_aabb) {}
    };

}

