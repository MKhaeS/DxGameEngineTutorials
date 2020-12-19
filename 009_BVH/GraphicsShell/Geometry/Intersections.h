#ifndef _INTERSECTIONS_H
#define _INTERSECTIONS_H

#include <algorithm>

#include "Ray.h"
#include "Extent.h"
#include "IntersectionOutput.h"
#include "Vector2.h"
#include "Parameters.h"
#include "Polygon.h"
#include "Bvh.h"


namespace Geometry
{
    inline bool RayExtentIntersection   (const SmartRay& ray, const Extent& extent, float* t);
    inline bool RayPolygonIntersection  (const SmartRay& ray, const Polygon& polygon,
                                         const bool& catch_backside, IntersectionOutput* output);
    inline IntersectionOutput TraceRay  (const SmartRay& r, const Bvh& bvh);
}



bool Geometry::RayExtentIntersection(const SmartRay & ray, const Extent & extent, float * t) {
    *t = FLT_MAX;
    float txmin, txmax, tymin, tymax, tzmin, tzmax, tmin, tmax;
    tmin = (extent.Min.X - ray.Origin().X) * ray.Denom().X;
    tmax = (extent.Max.X - ray.Origin().X) * ray.Denom().X;
    if (tmin < 0 && tmax < 0)
        return false;
    txmin = std::min(tmin, tmax);
    txmax = std::max(tmin, tmax);

    tmin = (extent.Min.Y - ray.Origin().Y) * ray.Denom().Y;
    tmax = (extent.Max.Y - ray.Origin().Y) * ray.Denom().Y;
    if (tmin < 0 && tmax < 0)
        return false;
    tymin = std::min(tmin, tmax);
    tymax = std::max(tmin, tmax);
    tmax = (extent.Max.Z - ray.Origin().Z) * ray.Denom().Z;
    tmin = (extent.Min.Z - ray.Origin().Z) * ray.Denom().Z;
    if (tmin < 0 && tmax < 0)
        return false;
    tzmin = std::min(tmin, tmax);
    tzmax = std::max(tmin, tmax);

    tmin = std::max(std::max(txmin, tymin), tzmin);
    tmax = std::min(std::min(txmax, tymax), tzmax);

    if (tmax >= tmin) {
        if (tmin >= 0)
            *t = tmin;
        else
            *t = 0;
        return true;
    }
    return false;
}



bool Geometry::RayPolygonIntersection(const SmartRay & ray, const Polygon & polygon,
                                      const bool & catch_backside, IntersectionOutput * output) {
    float t;
    Vector2 uv { 0.0f, 0.0f };

    Vector3 P   = Cross(ray.Direction(), polygon.m_E2);
    float det   = Dot(polygon.m_E1, P);

    if (det > -Parameters::FLOAT_DELTA && det < Parameters::FLOAT_DELTA) {
        return false;
    }

    float inv_det = 1 / det;

    Vector3 T = ray.Origin() - polygon.m_V1.Position;

    uv.X = Geometry::Dot(T, P) * inv_det;

    if (uv.X < -Parameters::FLOAT_DELTA || uv.Y > 1.0 + Parameters::FLOAT_DELTA) {
        return false;
    }
    Vector3 Q = Cross(T, polygon.m_E1);
    uv.Y = Dot(ray.Direction(), Q) * inv_det;

    if (uv.Y < -Parameters::FLOAT_DELTA || uv.X + uv.Y > 1.0f + Parameters::FLOAT_DELTA) {
        return false;
    }

    t = Dot(polygon.m_E2, Q) * inv_det;

    if (t > Parameters::FLOAT_DELTA) {
        output->IntersectionPoint = ray.Origin() + ray.Direction() * t;
        output->Visible = true;
        output->Distance = t;
        output->Uv = uv;
        output->Polygon = &polygon;
        return true;
    }
    return false;
}

inline Geometry::IntersectionOutput Geometry::TraceRay(const SmartRay& r, const Bvh& bvh) {
    std::stack<Geometry::Bvh*> node_stack;
    Geometry::Bvh* left_node = bvh.LeftNode.get();
    Geometry::Bvh* right_node = bvh.RightNode.get();
    if (left_node != nullptr)
        node_stack.push(bvh.LeftNode.get());
    if (right_node != nullptr)
        node_stack.push(bvh.RightNode.get());

    float tmin = FLT_MAX;
    IntersectionOutput best_output = {};
    int iteration = 0;
    while (!node_stack.empty()) {
        Bvh* nodes_to_process[2];
        nodes_to_process[0] = node_stack.top();
        node_stack.pop();
        nodes_to_process[1] = node_stack.top();
        node_stack.pop();
        float t[2];
        bool b_intersect0 = RayExtentIntersection(r, nodes_to_process[0]->Boundary, &t[0]);
        bool b_intersect1 = RayExtentIntersection(r, nodes_to_process[1]->Boundary, &t[1]);

        int n_nodes = 0;

        if (b_intersect0 && t[0] < tmin) {
            if (b_intersect1 && t[1] < tmin) {
                n_nodes = 2;
                if (t[1] < t[0]) { // if node[1] is closer, process it first
                    Bvh* temp_node = nodes_to_process[0];
                    nodes_to_process[0] = nodes_to_process[1];
                    nodes_to_process[1] = temp_node;
                    std::swap(t[0], t[1]);
                }
            } else {
                n_nodes = 1;
            }
        } else {
            if (b_intersect1 && t[1] < tmin) {
                n_nodes = 1;
                nodes_to_process[0] = nodes_to_process[1];
                t[0] = t[1];
            } else {
                continue;
            }
        }

        Polygon* best_polygon;

        Bvh* result_nodes[2] = { nullptr, nullptr };
        int n_result_nodes = 0;

        for (int n = 0; n < n_nodes; ++n) {
            if (t[n] > tmin) continue;
            // If no extents then this node isn't a leaf,
            // so add this node to further processing
            if (nodes_to_process[n]->Extents.size() == 0) {
                result_nodes[n_result_nodes] = nodes_to_process[n];
                ++n_result_nodes;
                continue;
            }
            for (int ex = 0; ex < nodes_to_process[n]->Extents.size(); ++ex) {
                float t;
                PolygonExtent cur_ext = nodes_to_process[n]->Extents[ex];
                if (!Geometry::RayExtentIntersection(r, cur_ext, &t)) continue;
                IntersectionOutput cur_output;

                if (!Geometry::RayPolygonIntersection(r, *cur_ext.LinkedPolygon, true, &cur_output)) continue;
                if (cur_output.Distance < tmin) {
                    best_polygon = cur_ext.LinkedPolygon;
                    best_output = cur_output;
                    tmin = cur_output.Distance;
                }
            }
        } // !for (int n = 0; n < n_nodes; ++n)
        if (n_result_nodes == 2) {
            node_stack.push(result_nodes[1]->LeftNode.get());
            node_stack.push(result_nodes[1]->RightNode.get());
            node_stack.push(result_nodes[0]->LeftNode.get());
            node_stack.push(result_nodes[0]->RightNode.get());
        } else if (n_result_nodes == 1) {
            node_stack.push(result_nodes[0]->LeftNode.get());
            node_stack.push(result_nodes[0]->RightNode.get());
        }
        ++iteration;
    } // !while(!node_stack.empty())
    return best_output;
}


#endif // !_INTERSECTIONS_H