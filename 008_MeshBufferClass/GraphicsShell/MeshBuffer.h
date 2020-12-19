#pragma once

#include "..\DxShell\DxShell.h"

#include "Geometry/Vertex.h"
#include "Geometry/AABB.h"

#include "Mesh.h"

constexpr auto BUFFER_SIZE   = 10000;
constexpr auto NUM_MESHES    = 100;
constexpr auto NUM_INSTANCES = 100;

class MeshBuffer {

private:
    friend class GraphicsShell;

    static void                Initialize ( );
    static void                DeInitialize ( );
    static Mesh*               CreateMesh ( );
    static MeshInstance*       CreateMeshInstance ( Mesh* mesh );
    static Geometry::FaceAabb* CreateFaceAabb ( );

    static inline DxBuffer* vertexBuffer      = nullptr;
    static inline DxBuffer* indexBuffer       = nullptr;
    static inline DxBuffer* modelMatrixBuffer = nullptr;

    static inline Geometry::Vertex3d*  vbPointer                = nullptr;
    static inline int*                 ibPointer                = nullptr;
    static inline DirectX::XMFLOAT4X4* modelMatrixBufferPointer = nullptr;
    
    static inline Mesh*         meshes           = nullptr;
    static inline MeshInstance* meshInstances    = nullptr;
    static inline int           numMeshes        = 0;

    static inline Geometry::FaceAabb* faceAabbs  = nullptr;
    static inline int                 numAabbs   = 0;
};