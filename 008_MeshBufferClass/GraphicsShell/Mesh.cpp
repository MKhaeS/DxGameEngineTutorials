#include "Mesh.h"


void MeshInstance::Translate ( float x, float y, float z ) {
    auto translate_matrix = DirectX::XMMatrixTranslation ( x, y, z );
    DirectX::XMStoreFloat4x4 ( &positionMatrix, translate_matrix );
}

void MeshInstance::UpdateModelMatrix ( ) {
    auto model_matrix = DirectX::XMLoadFloat4x4 ( &positionMatrix );
    model_matrix = DirectX::XMMatrixTranspose ( model_matrix );
    DirectX::XMStoreFloat4x4 ( modelMatrixPointer, model_matrix );
}
