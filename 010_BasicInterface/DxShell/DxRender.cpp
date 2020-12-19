#include "DxShell.h"

void DxShell::BindShader ( const DxShader * shader ) {
    cmdList->IASetPrimitiveTopology ( shader->primitiveTopology );
    cmdList->SetGraphicsRootSignature ( shader->rootSignature.Get() );
    cmdList->SetPipelineState ( shader->pipelineState.Get ( ) );
}

void DxShell::BindVertexBuffer ( const DxBuffer * buffer ) {
    if (buffer->type == DxBuffer::Type::GPU_BUFFER &&
        buffer->currentState != D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER ) {
        return;
    }
    cmdList->IASetVertexBuffers ( 0, 1, &buffer->vertexBufferView );
}

void DxShell::BindVertexBuffers ( const DxBuffer * v_buffer, 
                                  const DxBuffer * inst_buffer ) {
    if ( v_buffer->type == DxBuffer::Type::GPU_BUFFER &&
         v_buffer->currentState != D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER ) {
        return;
    }
    if ( inst_buffer->type == DxBuffer::Type::GPU_BUFFER &&
         inst_buffer->currentState != D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER ) {
        return;
    }

    D3D12_VERTEX_BUFFER_VIEW vb_views[2];
    vb_views[0] = v_buffer->vertexBufferView;
    vb_views[1] = inst_buffer->vertexBufferView;

    cmdList->IASetVertexBuffers ( 0, 2, vb_views );
}

void DxShell::BindIndexBuffer ( const DxBuffer * buffer ) {
    if ( buffer->type == DxBuffer::Type::GPU_BUFFER &&
         buffer->currentState != D3D12_RESOURCE_STATE_INDEX_BUFFER ) {
        return;
    }
    cmdList->IASetIndexBuffer ( &buffer->indexBufferView );
}

void DxShell::BindConstants ( UINT parameter_idx, UINT size, void * data, UINT offset ) {
    cmdList->SetGraphicsRoot32BitConstants ( parameter_idx, size, data, offset );
}

void DxShell::BindCBV ( UINT parameter_idx, const DxBuffer* buffer ) {
    cmdList->SetGraphicsRootConstantBufferView ( parameter_idx, buffer->gpuAddress );
}

void DxShell::BindTextureArray ( UINT parameter_idx, const DxTexture * texture ) {
    cmdList->SetGraphicsRootDescriptorTable ( parameter_idx, texture->srvGpuHandle );
}

void DxShell::DrawIndexed ( int num_indices, int num_instances, int vb_offset, int ib_offset ) {
    cmdList->DrawIndexedInstanced ( num_indices, num_instances, vb_offset, ib_offset, 0 );
}

void DxShell::Draw ( int num_vertices, int num_instances, int vb_offset ) {
    cmdList->DrawInstanced (num_vertices, num_instances, vb_offset, 0);
}
