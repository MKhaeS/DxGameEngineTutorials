#pragma once

namespace Geometry {
    struct Vector3 {
        union {
            float Data[3] = { 0.0f, 0.0f, 0.0f };
            struct {
                float X;
                float Y;
                float Z;
            };
        };


        inline float& operator[]( int i ) {
            return Data[i]; // no check for perfomance benefit
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