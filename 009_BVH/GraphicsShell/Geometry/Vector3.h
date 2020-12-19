#pragma once

#include <DirectXMath.h>

namespace Geometry {
    struct Vector3 {
        union {
            float data[3] = { 0.0f, 0.0f, 0.0f };
            struct {
                float x;
                float y;
                float z;
            };
        };

        const inline float& operator[](const int idx) const {
            return data[idx]; // no check for perfomance benefit
        }

        inline float& operator[](const int idx) {
            return data[idx]; // no check for perfomance benefit
        }
    };


    namespace vec3 {
        const Vector3 Zero       = Vector3 { 0.0f, 0.0f, 0.0f };
        const Vector3 Unit       = Vector3 { 1.0f, 1.0f, 1.0f };
        const Vector3 UnitX      = Vector3 { 1.0f, 0.0f, 0.0f };
        const Vector3 UnitY      = Vector3 { 0.0f, 1.0f, 0.0f };
        const Vector3 UnitZ      = Vector3 { 0.0f, 0.0f, 1.0f };
    }

    typedef Vector3 Point3;
    typedef Vector3 ColorF;

}