#include "GraphicsShell.h"

const Mesh* GraphicsShell::AddMesh ( std::string filename ) {
    auto box_mesh = Scene::CreateMeshFromFile ( filename );
    auto box_bvh = Scene::ConstructBvhForMesh ( box_mesh );
    return box_mesh;
}


MeshInstance * GraphicsShell::CreateMeshInstance ( const Mesh * mesh ) {
    auto mesh_instance = Scene::CreateMeshInstance ( mesh );
    return mesh_instance;
}

const void GraphicsShell::SetMeshInstancePosition ( MeshInstance * mesh_instance, 
                                                    float x, float y, float z ) {
    Scene::SetMeshInstancePosition ( mesh_instance, x, y, z );
}

void GraphicsShell::MeshInstanceRotateY ( MeshInstance * mesh_instance, float angle ) {
    Scene::MeshInstanceRotateY ( mesh_instance, angle );
}

void GraphicsShell::UpdateMeshInstanceModelMatrix ( MeshInstance * mesh_instance ) {
    Scene::UpdateMeshInstanceModelMatrix ( mesh_instance );
    AddAabbToDebugGeometry ( mesh_instance->GetAabb ( ), { 0.1f, 0.8f, 0.9f } );
}

