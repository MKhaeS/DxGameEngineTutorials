#include "Mesh.h"
#include "Scene.h"


void MeshInstance::SetPosition ( float x, float y, float z ) {
    Scene::SetMeshInstancePosition (this, x, y, z);
}

void MeshInstance::SetRotationY ( float angle ) {
    Scene::MeshInstanceRotateY ( this, angle );
}

void MeshInstance::UpdateModelMatrix ( ) {
    Scene::UpdateMeshInstanceModelMatrix ( this );
    /*auto translate_matrix = DirectX::XMLoadFloat4x4 ( &positionMatrix );
    auto rotate_matrix    = DirectX::XMLoadFloat4x4 ( &rotationMatrix );

    auto model_matrix = rotate_matrix * translate_matrix;
    model_matrix = DirectX::XMMatrixTranspose ( model_matrix );
    DirectX::XMStoreFloat4x4 ( modelMatrixPointer, model_matrix );*/
}

const void Scene::SetMeshInstancePosition ( MeshInstance* mesh_instance,
                                            float x, float y, float z ) {
    auto translate_matrix = DirectX::XMMatrixTranslation ( x, y, z );

    auto& pos_matrix = mesh_instance->positionMatrix;
    DirectX::XMStoreFloat4x4 ( &pos_matrix, translate_matrix );

    auto& mesh_instance_aabb = meshInstanceAabbs[mesh_instance->aabbOffset];
    auto mesh_aabb = meshes[mesh_instance_aabb.meshIdx].aabb;

    /*mesh_instance_aabb.Min.x = mesh_aabb.Min.x + x;
    mesh_instance_aabb.Min.y = mesh_aabb.Min.y + y;
    mesh_instance_aabb.Min.z = mesh_aabb.Min.z + z;

    mesh_instance_aabb.Max.x = mesh_aabb.Max.x + x;
    mesh_instance_aabb.Max.y = mesh_aabb.Max.y + y;
    mesh_instance_aabb.Max.z = mesh_aabb.Max.z + z;*/
}

void Scene::MeshInstanceRotateY ( MeshInstance* mesh_instance, float angle ) {
    auto rotate_matrix = DirectX::XMMatrixRotationY ( angle );

    auto& rotation_matrix = mesh_instance->rotationMatrix;
    DirectX::XMStoreFloat4x4 ( &rotation_matrix, rotate_matrix );


}

void Scene::UpdateMeshInstanceModelMatrix ( MeshInstance* mesh_instance ) {
    auto translate_matrix = DirectX::XMLoadFloat4x4 ( &mesh_instance->positionMatrix );
    auto rotate_matrix    = DirectX::XMLoadFloat4x4 ( &mesh_instance->rotationMatrix );

    auto model_matrix = rotate_matrix * translate_matrix;
    model_matrix = DirectX::XMMatrixTranspose ( model_matrix );
    DirectX::XMStoreFloat4x4 ( mesh_instance->modelMatrixPointer, model_matrix );

    // -- !!Refactor --
    model_matrix = DirectX::XMMatrixTranspose ( model_matrix );

    auto linked_mesh = mesh_instance->linked_mesh;
    auto& mesh_instance_aabb = meshInstanceAabbs[mesh_instance->aabbOffset];
    mesh_instance_aabb.Min = { FLT_MAX,  FLT_MAX,  FLT_MAX };
    mesh_instance_aabb.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    auto vb_pointer = vbPointer + linked_mesh->vbOffset;
    auto ib_pointer = ibPointer + linked_mesh->ibOffset;

    auto ib_size = linked_mesh->ibSize;

    for ( int i = 0; i < ib_size; i++ ) {
        auto cur_vertex = vb_pointer[ib_pointer[i]];
        auto vertex_position = cur_vertex.Position;
        DirectX::XMFLOAT4 vertex_normalized_position = {
            vertex_position.x,
            vertex_position.y,
            vertex_position.z,
            1.0f
        };
        auto vertex_position_vector = DirectX::XMLoadFloat4 ( &vertex_normalized_position );
        auto new_vertex_position_vector = DirectX::XMVector4Transform(vertex_position_vector, model_matrix);
        DirectX::XMStoreFloat3 ( &vertex_position, new_vertex_position_vector );


        if ( vertex_position.x < mesh_instance_aabb.Min.x ) {
            mesh_instance_aabb.Min.x = vertex_position.x;
        }
        if ( vertex_position.y < mesh_instance_aabb.Min.y ) {
            mesh_instance_aabb.Min.y = vertex_position.y;
        }
        if ( vertex_position.z < mesh_instance_aabb.Min.z ) {
            mesh_instance_aabb.Min.z = vertex_position.z;
        }
        if ( vertex_position.x > mesh_instance_aabb.Max.x ) {
            mesh_instance_aabb.Max.x = vertex_position.x;
        }
        if ( vertex_position.y > mesh_instance_aabb.Max.y ) {
            mesh_instance_aabb.Max.y = vertex_position.y;
        }
        if ( vertex_position.z > mesh_instance_aabb.Max.z ) {
            mesh_instance_aabb.Max.z = vertex_position.z;
        }
    }
}


const Geometry::Aabb & MeshInstance::GetAabb ( ) const {
    return Scene::GetMeshInstanceAabb ( this );
}




