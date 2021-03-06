#pragma once

#include <DirectXMath.h>

#include "../AABB.h"

class Mesh {
public:
    const Geometry::Aabb& GetAabb () const {
        return aabb;
    }
private:
    friend class Scene;

    Mesh ()                       = default;
    ~Mesh ()                      = default;
    Mesh (const Mesh&)            = delete;
    Mesh (Mesh&&)                 = delete;
    Mesh& operator= (const Mesh&) = delete;
    Mesh& operator= (Mesh&&)      = delete;

    int vbOffset                = 0;
    int ibOffset                = 0;
    int vbSize                  = 0;
    int ibSize                  = 0;
    int modelMatrixBufferOffset = 0;
    int instancesOffset         = 0;
    int numInstances            = 0;
    int facesAabbOffset         = 0;
    int bvhTopNodeOffset        = -1;
    Geometry::Aabb aabb         = { {  FLT_MAX,  FLT_MAX,  FLT_MAX},
                                    { -FLT_MAX, -FLT_MAX, -FLT_MAX} };
};

struct MeshInstance {
    void SetPosition       ( float x, float y, float z );
    void UpdateModelMatrix ( );
private:
    friend class Scene;

    MeshInstance ()                               = default;
    ~MeshInstance ()                              = default;
    MeshInstance (const MeshInstance&)            = delete;
    MeshInstance (MeshInstance&&)                 = delete;
    MeshInstance& operator= (const MeshInstance&) = delete;
    MeshInstance& operator= (MeshInstance&&)      = delete;

    int id     = 0;
    int meshId = 0;
    
    DirectX::XMFLOAT4X4 rotationMatrix = {};
    DirectX::XMFLOAT4X4 positionMatrix = {};

    DirectX::XMFLOAT4X4* modelMatrixPointer = nullptr;

    int aabbOffset = 0;
};

