#pragma once

#include "DxInclude.h"


class DxBuffer {
public:
    enum class Type { GPU_BUFFER, CPU_BUFFER, UNKNOWN };

    const UINT size = 0;
    const Type type   = Type::CPU_BUFFER;

    void  SetAsVertexBuffer (UINT stride);
    void  SetAsIndexBuffer ( );
    void* GetDataPointer ( ) const;
    void  UnmapFromCpuMemory ( );
    
    ~DxBuffer ( ) = default;
private:
    friend class DxShell;

    DxBuffer ( ) = default;
    DxBuffer ( UINT size, Type buffer_type );
    
    ComPtr<ID3D12Resource>      resource            = nullptr;
    D3D12_RESOURCE_DESC         description         = {};
    D3D12_HEAP_PROPERTIES       heapProperties      = {};
    union {
        D3D12_VERTEX_BUFFER_VIEW vertexBufferView   = {};
        D3D12_INDEX_BUFFER_VIEW  indexBufferView;
    };
    D3D12_GPU_VIRTUAL_ADDRESS   gpuAddress          = {};
    D3D12_RESOURCE_STATES       currentState        = {};
    void*                       dataPointer         = nullptr;
};