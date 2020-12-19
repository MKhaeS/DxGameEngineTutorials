#include "DxShell.h"

DxBuffer::DxBuffer ( UINT size, Type buffer_type )
    :
    size ( size ), type ( buffer_type ) {

    description.DepthOrArraySize = 1;
    description.Dimension        = D3D12_RESOURCE_DIMENSION_BUFFER;
    description.Layout           = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    description.MipLevels        = 1;
    description.SampleDesc.Count = 1;
    description.Width            = size;
    description.Height           = 1;

    if ( buffer_type == Type::CPU_BUFFER ) {
        currentState        = D3D12_RESOURCE_STATE_GENERIC_READ;
        heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
    } else {
        currentState        = D3D12_RESOURCE_STATE_COPY_DEST;
        heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
    }
}

void DxBuffer::SetAsVertexBuffer ( UINT stride ) {
    vertexBufferView.BufferLocation = resource->GetGPUVirtualAddress ( );
    vertexBufferView.SizeInBytes = size;
    vertexBufferView.StrideInBytes = stride;
}

void * DxBuffer::GetDataPointer ( ) const {
    return dataPointer;
}

void DxBuffer::UnmapFromCpuMemory ( ) { 
    resource->Unmap ( 0, 0 );
}

DxBuffer* DxShell::CreateBuffer ( UINT size, DxBuffer::Type buffer_type ) {
    DxBuffer buffer ( size, buffer_type );

    opResult = device->CreateCommittedResource ( &buffer.heapProperties,
                                                 D3D12_HEAP_FLAG_NONE,
                                                 &buffer.description,
                                                 buffer.currentState,
                                                 nullptr,
                                                 IID_PPV_ARGS ( buffer.resource.GetAddressOf ( ) ) );
    if ( FAILED ( opResult ) ) {
        LogError ( opResult, "Failed to create the buffer, HRESULT = " );
        return nullptr;
    }
    opResult = buffer.resource->Map ( 0, 0, &buffer.dataPointer );
    if ( FAILED ( opResult ) ) {
        LogError ( opResult, "Failed to map the buffer, HRESULT = " );
    }
    buffer.gpuAddress = buffer.resource->GetGPUVirtualAddress ( );
    dataBuffers.emplace_back ( buffer );
    return &dataBuffers.back();
}


