#include "MeshBuffer.h"

void MeshBuffer::Initialize ( ) {
    vertexBuffer = DxShell::CreateBuffer ( BUFFER_SIZE * sizeof ( Geometry::Vertex2d ), DxBuffer::Type::CPU_BUFFER );
    indexBuffer  = DxShell::CreateBuffer ( BUFFER_SIZE * sizeof ( int ),      DxBuffer::Type::CPU_BUFFER );

    vbPointer = reinterpret_cast<Geometry::Vertex3d*>(vertexBuffer->GetDataPointer ( ));
    ibPointer = reinterpret_cast<int*>     (indexBuffer->GetDataPointer ( ));

    vertexBuffer->SetAsVertexBuffer ( sizeof ( Geometry::Vertex3d ) );
    indexBuffer->SetAsIndexBuffer ( );

    modelMatrixBuffer = DxShell::CreateBuffer ( NUM_INSTANCES * NUM_MESHES * sizeof ( DirectX::XMFLOAT4X4 ),
                                                DxBuffer::Type::CPU_BUFFER );
    modelMatrixBufferPointer = reinterpret_cast<DirectX::XMFLOAT4X4*>(modelMatrixBuffer->GetDataPointer ( ));

    meshes        = new Mesh[NUM_MESHES];
    meshInstances = new MeshInstance[NUM_MESHES*NUM_INSTANCES];
    faceAabbs     = new Geometry::FaceAabb[BUFFER_SIZE / 3];
}

void MeshBuffer::DeInitialize ( ) {
    delete[ ] meshes;
    delete[ ] meshInstances;
    delete[ ] faceAabbs;
}

Mesh* MeshBuffer::CreateMesh ( ) {
    meshes[numMeshes].modelMatrixBufferOffset = numMeshes * NUM_INSTANCES;
    meshes[numMeshes].id = numMeshes;
    return &meshes[numMeshes++];
}

MeshInstance * MeshBuffer::CreateMeshInstance ( Mesh * mesh ) {
    int mesh_id = mesh->id;
    int instance_id = mesh->numInstances;

    int instance_offset = mesh_id * NUM_INSTANCES + instance_id;
    meshInstances[instance_offset].modelMatrixPointer = modelMatrixBufferPointer + instance_offset;

    auto identity_matrix = DirectX::XMMatrixIdentity ( );
    DirectX::XMStoreFloat4x4 ( meshInstances[instance_offset].modelMatrixPointer, identity_matrix );
    DirectX::XMStoreFloat4x4 ( &meshInstances[instance_offset].positionMatrix, identity_matrix );

    mesh->numInstances++;
    return &meshInstances[instance_offset];
}

Geometry::FaceAabb * MeshBuffer::CreateFaceAabb ( ) {
    return &faceAabbs[numAabbs++];
}
