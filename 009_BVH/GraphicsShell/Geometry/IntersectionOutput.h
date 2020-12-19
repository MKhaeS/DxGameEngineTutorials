#ifndef _INTERSECTIONOUTPUT_H
#define _INTERSECTIONOUTPUT_H

#include "Vector3.h"
#include "Polygon.h"


namespace Geometry
{
    struct IntersectionOutput
    {
        const Polygon* Polygon;
        Vector3 IntersectionPoint;
        Vector2 Uv;
        float Distance;
        bool Visible;
    };
}
#endif // !_INTERSECTIONOUTPUT_H