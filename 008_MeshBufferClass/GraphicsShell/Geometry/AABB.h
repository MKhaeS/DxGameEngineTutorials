#pragma once


#include "Vector3.h"

namespace Geometry {
    struct Aabb {
        Vector3 Min = { 0.0f, 0.0f, 0.0f };
        Vector3 Max = { 0.0f, 0.0f, 0.0f };
    };

    struct FaceAabb : public Aabb {
        int faceOffset = 0;
    };
}

