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
    vertexBufferView.BufferLocation = gpuAddress;
    vertexBufferView.SizeInBytes = size;
    vertexBufferView.StrideInBytes = stride;
}

void DxBuffer::SetAsIndexBuffer ( ) {
    indexBufferView.BufferLocation = gpuAddress;
    indexBufferView.SizeInBytes = size;
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void * DxBuffer::GetDataPointer ( ) const {
    return dataPointer;
}

void DxBuffer::UnmapFromCpuMemory ( ) { 
    resource->Unmap ( 0, 0 );
}

DxBuffer* DxShell::CreateBuffer ( UINT size, DxBuffer::Type buffer_type ) {
    if ( buffer_type == DxBuffer::Type::UNKNOWN ) {
        return nullptr;
    }
    DxBuffer* buffer = &dataBuffers[num_buffers];
    buffer = new (buffer) DxBuffer ( size, buffer_type );
    num_buffers++;

    opResult = device->CreateCommittedResource ( &buffer->heapProperties,
                                                 D3D12_HEAP_FLAG_NONE,
                                                 &buffer->description,
                                                 buffer->currentState,
                                                 nullptr,
                                                 IID_PPV_ARGS ( buffer->resource.GetAddressOf ( ) ) );
    if ( FAILED ( opResult ) ) {
        LogError ( opResult, "Failed to create the buffer, HRESULT = " );
        return nullptr;
    }
    
    if ( buffer_type == DxBuffer::Type::CPU_BUFFER ) {
        opResult = buffer->resource->Map ( 0, 0, &buffer->dataPointer );
        if ( FAILED ( opResult ) ) {
            LogError ( opResult, "Failed to map the buffer, HRESULT = " );
        }
    }

    buffer->gpuAddress = buffer->resource->GetGPUVirtualAddress ( );
    return buffer;
}

void DxShell::ChangeBufferState ( DxBuffer * buffer, D3D12_RESOURCE_STATES new_state ) {
    if ( buffer->currentState == new_state ) {
        return;
    }
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource   = buffer->resource.Get();
    barrier.Transition.StateBefore = buffer->currentState;
    barrier.Transition.StateAfter  = new_state;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    // !!! - Not effective, batch the barriers -- 
    cmdList->ResourceBarrier ( 1, &barrier );
    buffer->currentState = new_state;
}



void DxShell::CopyBuffer ( DxBuffer * dst_buffer, DxBuffer * src_buffer, 
                           UINT size, UINT dst_offset, UINT src_offset ) {
    cmdList->CopyBufferRegion( dst_buffer->resource.Get(), dst_offset,
                               src_buffer->resource.Get(), src_offset,
                               size );
}


