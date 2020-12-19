#include "DxShell.h"

void DxShell::BindShader ( DxShader * shader ) {
    cmdList->IASetPrimitiveTopology ( shader->primitiveTopology );
    cmdList->SetGraphicsRootSignature ( shader->rootSignature.Get() );
    cmdList->SetPipelineState ( shader->pipelineState.Get ( ) );
}

void DxShell::BindBuffer ( DxBuffer * buffer ) {
    cmdList->IASetVertexBuffers ( 0, 1, &buffer->vertexBufferView );
}

void DxShell::DrawInstanced ( int num_vertices ) {

    cmdList->DrawInstanced ( num_vertices, 1, 0, 0 );
}