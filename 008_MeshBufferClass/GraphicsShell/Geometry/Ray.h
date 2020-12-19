#ifndef _RAY_H
#define _RAY_H

#include "Vector3.h"

namespace Geometry
{
    class Ray
    {
    public:
        inline         Ray          (const Vector3& origin, const Vector3& direction);
        inline void    Origin       (const Vector3& origin);
        inline void    Direction    (const Vector3& direction);

        inline const Vector3& Origin()    const;
        inline const Vector3& Direction() const;        

    protected:
        Vector3 m_Origin;
        Vector3 m_Direction;
    };

    inline Ray::Ray(const Vector3 & origin, const Vector3 & direction)
    : m_Origin(origin), m_Direction(direction) {
        Normalize(&m_Direction);
    }

    inline void Ray::Origin(const Vector3& origin) {
        m_Origin = origin;
    }

    inline void Ray::Direction(const Vector3& direction) {
        m_Direction = direction;
    }

    inline const Vector3& Ray::Origin() const {
        return m_Origin;
    }

    inline const Vector3& Ray::Direction() const {
        return m_Direction;
    }


    class SmartRay : public Ray
    {
    public:
        inline          SmartRay     (const Vector3& origin, const Vector3& direction);        
        inline void     Direction    (const Vector3& direction);

        inline const    Vector3& Direction() const;
        inline const    Vector3&  Denom()              const;
        inline Vector3& Intencity();
        inline Vector3& LightImpact();
        inline int&     BouncesLeft();
        inline float&   RefractionIndex();
        

    private:
        Vector3             m_Denom;
        Vector3             m_Intencity;
        Vector3             m_LightImpact;
        int                 m_BouncesLeft;
        float               m_RefractionIndex = 1.0f;
    };


    inline SmartRay::SmartRay(const Vector3& origin, const Vector3& direction)
    : Ray(origin, direction) {
        m_Denom.X = 1 / direction.X;
        m_Denom.Y = 1 / direction.Y;
        m_Denom.Z = 1 / direction.Z;
    }
    

    inline void SmartRay::Direction(const Vector3& direction) {
        m_Direction = direction;
        Normalize(&m_Direction);
        m_Denom.Y = 1 / m_Direction.Y;
        m_Denom.Z = 1 / m_Direction.Z;
        m_Denom.X = 1 / m_Direction.X;
    }

    inline const Vector3&  SmartRay::Direction() const {
        return m_Direction;
    }


    inline const Vector3& SmartRay::Denom() const {
        return m_Denom;
    }


    inline int& SmartRay::BouncesLeft() {
        return m_BouncesLeft;
    }


    inline Vector3& SmartRay::Intencity() {
        return m_Intencity;
    }


    inline Vector3& SmartRay::LightImpact() {
        return m_LightImpact;
    }


    inline float& SmartRay::RefractionIndex() {
        return m_RefractionIndex;
    }
}
#endif // !_RAY_H