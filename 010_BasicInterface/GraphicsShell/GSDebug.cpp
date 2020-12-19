#include "GraphicsShell.h"

#include "Geometry/Scene/Scene.h"
#include "Geometry/Scene/Mesh.h"



void GraphicsShell::InitDebugGeometryBuffers () {
    auto vb_buffer_size_in_bytes = DEBUG_GEOMETRY_BUFFER_SIZE * sizeof (Geometry::Vertex3dPosCol);
    debugGeometryVertexBuffer = DxShell::CreateBuffer (vb_buffer_size_in_bytes, DxBuffer::Type::CPU_BUFFER);
    debugGeometryVBPtr = reinterpret_cast<Geometry::Vertex3dPosCol*> (debugGeometryVertexBuffer->GetDataPointer());

    debugGeometryVertexBuffer->SetAsVertexBuffer (sizeof (Geometry::Vertex3dPosCol));
}

void GraphicsShell::AddAabbToDebugGeometry (const Geometry::Aabb& aabb, DirectX::XMFLOAT3 color) {
    auto vb_pointer = debugGeometryVBPtr;

    vb_pointer[0].Position  = { aabb.Min.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[1].Position  = { aabb.Max.x, aabb.Min.y, aabb.Min.z };

    vb_pointer[2].Position  = { aabb.Min.x, aabb.Min.y, aabb.Max.z };
    vb_pointer[3].Position  = { aabb.Max.x, aabb.Min.y, aabb.Max.z };

    vb_pointer[4].Position  = { aabb.Min.x, aabb.Max.y, aabb.Min.z };
    vb_pointer[5].Position  = { aabb.Max.x, aabb.Max.y, aabb.Min.z };

    vb_pointer[6].Position  = { aabb.Min.x, aabb.Max.y, aabb.Max.z };
    vb_pointer[7].Position  = { aabb.Max.x, aabb.Max.y, aabb.Max.z };



    vb_pointer[8].Position  = { aabb.Min.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[9].Position  = { aabb.Min.x, aabb.Max.y, aabb.Min.z };

    vb_pointer[10].Position = { aabb.Min.x, aabb.Min.y, aabb.Max.z };
    vb_pointer[11].Position = { aabb.Min.x, aabb.Max.y, aabb.Max.z };

    vb_pointer[12].Position = { aabb.Max.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[13].Position = { aabb.Max.x, aabb.Max.y, aabb.Min.z };

    vb_pointer[14].Position = { aabb.Max.x, aabb.Min.y, aabb.Max.z };
    vb_pointer[15].Position = { aabb.Max.x, aabb.Max.y, aabb.Max.z };



    vb_pointer[16].Position = { aabb.Min.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[17].Position = { aabb.Min.x, aabb.Min.y, aabb.Max.z };

    vb_pointer[18].Position = { aabb.Max.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[19].Position = { aabb.Max.x, aabb.Min.y, aabb.Max.z };

    vb_pointer[20].Position = { aabb.Min.x, aabb.Max.y, aabb.Min.z };
    vb_pointer[21].Position = { aabb.Min.x, aabb.Max.y, aabb.Max.z };

    vb_pointer[22].Position = { aabb.Max.x, aabb.Max.y, aabb.Min.z };
    vb_pointer[23].Position = { aabb.Max.x, aabb.Max.y, aabb.Max.z };

    for (int i = 0; i < 24; i++) {
        vb_pointer[i].Color = color;
    }

    debugGeometryVBPtr += 24;
    numDebugLines += 12;
}

void GraphicsShell::AddBvhTreeToDebugGeometry (const Geometry::Bvh* bvh, 
                                               int min_depth, int max_depth, 
                                               DirectX::XMFLOAT3 color) {
    if (max_depth < 0) {
        return;
    }
    auto vb_pointer = debugGeometryVBPtr;

    if (min_depth <= 0) {
        AddAabbToDebugGeometry (bvh->GetBoundary (), color);
    }

    auto left_node = bvh->GetLeftNode ();
    auto right_node = bvh->GetRightNode ();

    if (left_node) {
        AddBvhTreeToDebugGeometry (left_node, min_depth - 1, max_depth - 1, { 0.0f, 1.0f, 0.0f });
    }

    if (right_node) {
        AddBvhTreeToDebugGeometry (right_node, min_depth - 1, max_depth - 1, { 1.0f, 0.0f, 0.0f });
    }
}

void GraphicsShell::AddRayToDebugGeometry ( const Geometry::Ray& ray ) {
    auto length_factor = 1000.0f;
    auto vb_pointer = debugGeometryVBPtr;
    vb_pointer[0].Position = ray.origin;
    vb_pointer[1].Position = { ray.origin.x + length_factor*ray.direction.x,
                               ray.origin.y + length_factor*ray.direction.y,
                               ray.origin.z + length_factor*ray.direction.z };

    vb_pointer[1].Color = { 0.4f, 0.1f, 0.8f };
    vb_pointer[0].Color = { 0.7f, 0.8f, 0.1f };


    debugGeometryVBPtr += 2;
    numDebugLines += 1;
}

void GraphicsShell::AddLightSourceToDebugGeometry ( const LightSource & light_source ) {
    float line_length = 10.0f;
    DirectX::XMFLOAT3 color = { 1.0f, 1.0f, 0.0f };
    auto vb_pointer = debugGeometryVBPtr;
    auto light_source_position = light_source.GetPosition ( );
    vb_pointer[0].Position = light_source_position;
    vb_pointer[1].Position = light_source_position;
    vb_pointer[2].Position = light_source_position;
    vb_pointer[3].Position = light_source_position;
    vb_pointer[4].Position = light_source_position;
    vb_pointer[5].Position = light_source_position;

    vb_pointer[0].Position.x -= line_length;
    vb_pointer[1].Position.x += line_length;
    vb_pointer[2].Position.y -= line_length;
    vb_pointer[3].Position.y += line_length;
    vb_pointer[4].Position.z -= line_length;
    vb_pointer[5].Position.z += line_length;


    vb_pointer[0].Color = color;
    vb_pointer[1].Color = color;
    vb_pointer[2].Color = color;
    vb_pointer[3].Color = color;
    vb_pointer[4].Color = color;
    vb_pointer[5].Color = color;

    debugGeometryVBPtr += 6;
    numDebugLines += 3;
}

void GraphicsShell::AddAxisToDebugGeometry ( ) {
    float line_length = 100;
    auto vb_pointer = debugGeometryVBPtr;
    vb_pointer[0].Position = { 0.0f, 0.0f, 0.0f };
    vb_pointer[1].Position = { line_length, 0.0f, 0.0f };
    vb_pointer[2].Position = { 0.0f, 0.0f, 0.0f };
    vb_pointer[3].Position = { 0.0f, line_length, 0.0f };
    vb_pointer[4].Position = { 0.0f, 0.0f, 0.0f };
    vb_pointer[5].Position = { 0.0f, 0.0f, line_length };

    vb_pointer[0].Color = { 1.0f, 0.0f, 0.0f };
    vb_pointer[1].Color = { 1.0f, 0.0f, 0.0f };
    vb_pointer[2].Color = { 0.0f, 1.0f, 0.0f };
    vb_pointer[3].Color = { 0.0f, 1.0f, 0.0f };
    vb_pointer[4].Color = { 0.0f, 0.0f, 1.0f };
    vb_pointer[5].Color = { 0.0f, 0.0f, 1.0f };

    debugGeometryVBPtr += 6;
    numDebugLines += 3;
}
