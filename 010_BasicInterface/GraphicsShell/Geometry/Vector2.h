#ifndef _VECTOR2_H
#define _VECTOR2_H

#include <math.h>

namespace Geometry
{
    struct Vector2
    {
        union
        {
            float Data[2] = { 0.0f, 0.0f };
            struct
            {
                float X;
                float Y;
            };
            struct
            {
                float U;
                float V;
            };
        };

        inline Vector2(const float& x = 0.0f, const float& y = 0.0f) : X(x), Y(y) {}

        inline float operator[](int i) {
            return Data[i]; // no check for perfomance benefit
        }
    };

    namespace vec2
    {
        const Vector2 Zero    = Vector2 { 0.0f, 0.0f };
        const Vector2 Unit    = Vector2 { 1.0f, 1.0f };
        const Vector2 UnitX   = Vector2 { 1.0f, 0.0f };
        const Vector2 UnitY   = Vector2 { 0.0f, 1.0f };
        const Vector2 UnitZ   = Vector2 { 0.0f, 0.0f };
    }

    typedef Vector2 Point2;


    inline Vector2 operator-(const Vector2& v) {
        return Vector2 { -v.X, -v.Y };
    }

    inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
        return Vector2 { v1.X + v2.X, v1.Y + v2.Y };
    }

    inline Vector2 operator-(const Vector2& v1, const Vector2& v2) {
        return Vector2 { v1.X - v2.X, v1.Y - v2.Y };
    }

    inline Vector2 operator*(const Vector2& v1, const Vector2& v2) {
        return Vector2 { v1.X * v2.X, v1.Y * v2.Y };
    }

    inline Vector2& operator*=(Vector2& v1, const Vector2& v2) {
        v1.X *= v2.X;
        v1.Y *= v2.Y;
        return v1;
    }

    inline Vector2 operator*(const Vector2& v, const float& m) {
        return Vector2 { v.X * m, v.Y * m };
    }

    inline Vector2 operator*(const float& m, const Vector2& v) {
        return Vector2 { v.X * m, v.Y * m };
    }

    inline float Dot(const Vector2& v1, const Vector2& v2) {
        return (v1.X * v2.X + v1.Y * v2.Y);
    }

    inline Vector3 Cross(const Vector2& v1, const Vector2& v2) {
        return Vector3 { v1.Y - v2.Y,
                         v2.X - v1.X,
                         v1.X * v2.Y - v1.Y * v2.X };
    }

    inline Vector2 Reflect(const Vector2& I, const Vector2& N) {
        return I - N * 2.0f * Dot(I, N);
    }

    inline float Length(const Vector2& v) {
        return sqrtf(v.X * v.X +
                     v.Y * v.Y );
    }

    inline void Normalize(Vector2* v) {
        float len = Length(*v);
        v->X /= len;
        v->Y /= len;
    }
}
#endif //!_VECTOR2_H
