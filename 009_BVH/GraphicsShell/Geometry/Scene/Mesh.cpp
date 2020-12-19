#include "Mesh.h"
#include "Scene.h"


void MeshInstance::SetPosition ( float x, float y, float z ) {
    Scene::SetMeshInstancePosition (this, x, y, z);
}

void MeshInstance::UpdateModelMatrix ( ) {
    auto model_matrix = DirectX::XMLoadFloat4x4 ( &positionMatrix );
    model_matrix = DirectX::XMMatrixTranspose ( model_matrix );
    DirectX::XMStoreFloat4x4 ( modelMatrixPointer, model_matrix );
}

