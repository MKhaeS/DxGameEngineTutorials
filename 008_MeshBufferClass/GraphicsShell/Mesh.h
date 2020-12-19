#pragma once

#include <DirectXMath.h>


struct Mesh {
private:
    friend class GraphicsShell;
    friend class MeshBuffer;

    int id                      = 0;
    int vbOffset                = 0;
    int ibOffset                = 0;
    int vbSize                  = 0;
    int ibSize                  = 0;
    int modelMatrixBufferOffset = 0;
    int numInstances            = 0;
    int aabbOffset              = 0;
};

struct MeshInstance {
    void Translate ( float x, float y, float z );
    void UpdateModelMatrix ( );
private:
    friend class MeshBuffer;

    int id     = 0;
    int meshId = 0;
    
    DirectX::XMFLOAT4X4 rotationMatrix = {};
    DirectX::XMFLOAT4X4 positionMatrix = {};

    DirectX::XMFLOAT4X4* modelMatrixPointer = nullptr;
};

