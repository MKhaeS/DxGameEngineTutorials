#ifndef _POLYGON_H
#define _POLYGON_H

#include <algorithm>

#include "Vertex.h"
#include "Extent.h"


class Shader;

namespace Geometry
{
    class SmartRay;
    class IntersectionOutput;

    bool RayPolygonIntersection(const SmartRay & ray, const Polygon & polygon,
                                const bool & catch_backside, IntersectionOutput * output);

    class Polygon
    {
    public:
        inline                      Polygon       (const Vertex& v1 = Vert::Null,
                                                   const Vertex& v2 = Vert::Null,
                                                   const Vertex& v3 = Vert::Null);
                                                  
        inline const Vertex&        Vert          (const int& i) const;
        inline const Vector3&       Normal        ()             const;
        inline const PolygonExtent& AABB          ()             const;
        inline const Shader*        GetShader ()           const;

        inline void                 SetShader (Shader* sh);

    private:
        union
        {
            Vertex m_Vert[3];
            struct
            {
                Vertex m_V1;
                Vertex m_V2;
                Vertex m_V3;
            };
        };

        int m_Id;

        Vector3 m_Normal;
        Shader* m_Shader;
        //Precalculated values for better perfomance
        friend bool  Geometry::RayPolygonIntersection(const SmartRay & ray, const Polygon & polygon,
                                                     const bool & catch_backside, IntersectionOutput * output);
        Vector3 m_E1, m_E2;
        float   m_L1, m_L2, m_L3;


        PolygonExtent m_Extent;
    };


    Polygon::Polygon(const Vertex & v1, const Vertex & v2, const Vertex & v3) {

        m_Vert[0] = v1;
        m_Vert[1] = v2;
        m_Vert[2] = v3;

        m_L1 = Length(v2.Position - v3.Position);
        m_L2 = Length(v3.Position - v1.Position);
        m_L3 = Length(v1.Position - v2.Position);

        static int ID = 0;
        m_Id = ID;
        ++ID;
        float min_x = std::min(v1.Position.X, std::min(v2.Position.X, v3.Position.X));
        float min_y = std::min(v1.Position.Y, std::min(v2.Position.Y, v3.Position.Y));
        float min_z = std::min(v1.Position.Z, std::min(v2.Position.Z, v3.Position.Z));
        float max_x = std::max(v1.Position.X, std::max(v2.Position.X, v3.Position.X));
        float max_y = std::max(v1.Position.Y, std::max(v2.Position.Y, v3.Position.Y));
        float max_z = std::max(v1.Position.Z, std::max(v2.Position.Z, v3.Position.Z));

        m_E1 = v2.Position - v1.Position;
        m_E2 = v3.Position - v1.Position;
        m_Normal = Cross(m_E1, m_E2);
        Normalize(&m_Normal);
        m_Extent.Min           = { min_x, min_y, min_z };
        m_Extent.Max           = { max_x, max_y, max_z };
        m_Extent.LinkedPolygon = this;
    }



    inline const Vertex& Polygon::Vert(const int& i) const {
        return m_Vert[i]; // no check for perfomance benefit
    }

    const Vector3& Polygon::Normal() const {
        return m_Normal;
    }

    const PolygonExtent& Polygon::AABB() const {
        return m_Extent;
    }

    const Shader* Polygon::GetShader() const {
        return m_Shader;
    }


    void Polygon::SetShader(Shader* sh) {
        m_Shader = sh;
    }
}
#endif // !_POLYGON_H
