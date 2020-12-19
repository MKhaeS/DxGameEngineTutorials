#include "DxShell.h"

void DxShell::BindShader ( DxShader * shader ) {
    cmdList->IASetPrimitiveTopology ( shader->primitiveTopology );
    cmdList->SetGraphicsRootSignature ( shader->rootSignature.Get() );
    cmdList->SetPipelineState ( shader->pipelineState.Get ( ) );
}

void DxShell::BindVertexBuffer ( DxBuffer * buffer ) {
    ChangeBufferState ( buffer, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER );
    cmdList->IASetVertexBuffers ( 0, 1, &buffer->vertexBufferView );
}

void DxShell::BindIndexBuffer ( DxBuffer * buffer ) {
    ChangeBufferState ( buffer, D3D12_RESOURCE_STATE_INDEX_BUFFER );
    cmdList->IASetIndexBuffer ( &buffer->indexBufferView );
}

void DxShell::BindConstants ( UINT parameter_idx, UINT size, void * data, UINT offset ) {
    cmdList->SetGraphicsRoot32BitConstants ( parameter_idx, size, data, offset );
}

void DxShell::BindCBV ( UINT parameter_idx, DxBuffer* buffer ) {
    cmdList->SetGraphicsRootConstantBufferView ( parameter_idx, buffer->gpuAddress );
}

void DxShell::BindTextureArray ( UINT parameter_idx, DxTexture * texture ) {
    cmdList->SetGraphicsRootDescriptorTable ( parameter_idx, texture->srvGpuHandle );
}

void DxShell::Draw ( int num_indices, int num_instances ) {
    cmdList->DrawIndexedInstanced ( num_indices, num_instances, 0, 0, 0 );
}