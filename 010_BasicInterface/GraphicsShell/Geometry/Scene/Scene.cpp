#include "Scene.h"

#include <fstream>
#include <algorithm>



void Scene::Initialize (int num_vertices, int num_indices, 
                        int num_meshes, int num_instances ) {
    auto vb_size = num_vertices * sizeof (Geometry::Vertex3d);
    auto ib_size = num_indices * sizeof (int);
    vertexBuffer = DxShell::CreateBuffer (vb_size, DxBuffer::Type::CPU_BUFFER);
    indexBuffer  = DxShell::CreateBuffer (ib_size, DxBuffer::Type::CPU_BUFFER);

    vbPointer = reinterpret_cast<Geometry::Vertex3d*>(vertexBuffer->GetDataPointer ());
    ibPointer = reinterpret_cast<int*>(indexBuffer->GetDataPointer ());

    vertexBuffer->SetAsVertexBuffer (sizeof (Geometry::Vertex3d));
    indexBuffer->SetAsIndexBuffer ();

    auto model_matrix_buffer_size = num_meshes * num_instances * sizeof (DirectX::XMFLOAT4X4);
    modelMatrixBuffer = DxShell::CreateBuffer (model_matrix_buffer_size, DxBuffer::Type::CPU_BUFFER);
    modelMatrixBufferPointer = reinterpret_cast<DirectX::XMFLOAT4X4*>(modelMatrixBuffer->GetDataPointer ());

    auto num_faces = num_vertices / 3;

    meshes            = new Mesh[num_meshes];
    meshInstances     = new MeshInstance[num_instances];
    meshInstanceAabbs = new Geometry::LinkedAabb[num_instances];    
    faceAabbs         = new Geometry::LinkedAabb[num_faces];
    faceBvhs          = new Geometry::Bvh[num_faces];
    meshBvhs          = new Geometry::Bvh[num_instances];
    lightSources      = new LightSource[100]; // !! REMOVE HARCODED VALUE

    InitializePbrShader ();
}

void Scene::DeInitialize () {
    delete[ ] faceBvhs;
    delete[ ] faceAabbs;
    delete[ ] meshInstanceAabbs;
    delete[ ] meshInstances;
    delete[ ] meshes;
    delete[ ] lightSources;
}

Mesh* Scene::CreateMeshFromFile ( std::string obj_filename ) {
    // !! MAKE A GPU BUFFER COPY OF THE GEOMETRY
    std::ifstream mesh_fs (obj_filename, std::ios::binary);
    if (mesh_fs.fail ()) {
        return nullptr;
    }

    auto new_mesh = InitMesh ();

    int size = 0;
    mesh_fs.read (reinterpret_cast<char*>(&size), 4);
    new_mesh->vbSize = size;
    new_mesh->ibSize = size;

    auto current_vb_ptr = vbPointer + curVbOffset;
    mesh_fs.read (reinterpret_cast<char*>(current_vb_ptr), size * sizeof (Geometry::Vertex3d));
    curVbOffset += size;

    // !! FIX INDICES (Currently mesh file has no indices and each vertex just corresponds to an index)
    auto indices  = ibPointer + curIbOffset;
    for (int i = 0; i < size; i++) {
        indices[i] = i;
    }
    curIbOffset += size;

    CalculateMeshFacesAabbs (new_mesh);



    new_mesh->instancesOffset = curMeshInstancesOffset;
    curMeshInstancesOffset += DEFAULT_NUM_INSTANCES;

    CalculateMeshAabb (new_mesh);

    //curMeshInstancesOffset += DEFAULT_NUM_INSTANCES;

    return new_mesh;
}

MeshInstance* Scene::CreateMeshInstance  ( const Mesh* mesh ) {
    int mesh_idx = (mesh - meshes);

    auto& mesh_num_instances = mesh->numInstances;

    auto instance_offset = mesh->instancesOffset + mesh_num_instances;
    auto new_mesh_instance = &meshInstances[instance_offset];
    new_mesh_instance->linked_mesh = const_cast<Mesh*>(mesh);
    new_mesh_instance->modelMatrixPointer = modelMatrixBufferPointer + instance_offset;

    auto instance_model_matrix_ptr = modelMatrixBufferPointer + instance_offset;

    auto identity_matrix = DirectX::XMMatrixIdentity ();
    DirectX::XMStoreFloat4x4 ( instance_model_matrix_ptr, identity_matrix );
    DirectX::XMStoreFloat4x4 ( &meshInstances[instance_offset].positionMatrix, identity_matrix );
    DirectX::XMStoreFloat4x4 ( &meshInstances[instance_offset].rotationMatrix, identity_matrix );

    meshInstanceAabbs[numMeshInstanceAabbs].Max     = mesh->aabb.Max;
    meshInstanceAabbs[numMeshInstanceAabbs].Min     = mesh->aabb.Min;
    meshInstanceAabbs[numMeshInstanceAabbs].meshIdx = mesh_idx;

    new_mesh_instance->aabbOffset = numMeshInstanceAabbs;

    numMeshInstanceAabbs++;
    mesh_num_instances++;

    return new_mesh_instance;
}

const DxBuffer * Scene::GetModelMatrixBuffer () {
    return modelMatrixBuffer;
}

const DxBuffer * Scene::GetVertexBuffer () {
    return vertexBuffer;
}

const DxBuffer * Scene::GetIndexBuffer () {
    return indexBuffer;
}

void Scene::RenderForCamera (const Camera* camera) {
    DxShell::BindShader (pbrShader);
    DxShell::BindCBV (0, camera->GetViewMatrixBuffer());
    DxShell::BindCBV (2, Scene::GetModelMatrixBuffer ());
    //DxShell::BindTextureArray (3, texture);

    DxShell::BindVertexBuffer (vertexBuffer);
    DxShell::BindIndexBuffer  (indexBuffer);
    

    for (int m = 0; m < Scene::numMeshes; m++) {
        DxShell::BindConstants (1, 1, &Scene::meshes[m].instancesOffset, 0);
        DxShell::DrawIndexed ( Scene::meshes[m].ibSize,
                               Scene::meshes[m].numInstances,
                               Scene::meshes[m].ibOffset,
                               Scene::meshes[m].vbOffset );
    }
}

const Geometry::Bvh * Scene::ConstructBvhForMesh (Mesh * mesh) {
    if (mesh == nullptr) {
        return nullptr;
    }

    auto mesh_face_aabbs_offset = mesh->facesAabbOffset;
    auto mesh_num_faces         = mesh->ibSize / 3;

    auto aabb_array = &faceAabbs[mesh_face_aabbs_offset];

    auto axis_sorted_aabbs = new Geometry::AxisSortedAabb[mesh_num_faces];
    for (int i = 0; i < mesh_num_faces; i++) {
        axis_sorted_aabbs[i] = aabb_array[i];
    }

    std::vector<Geometry::AxisSortedAabb*> axis_sorted_aabs_ptrs (mesh_num_faces);
    for (int i = 0; i < mesh_num_faces; i++) {
        axis_sorted_aabs_ptrs[i] =  &axis_sorted_aabbs[i];
    }
              
    auto top_node = InitFaceBvh ();

    ConstructBvhForAabbArray (axis_sorted_aabs_ptrs, top_node, InitFaceBvh);

    delete axis_sorted_aabbs;
    return top_node;
}

const Geometry::Bvh * Scene::ConstructSceneBvh () {

    auto aabb_array = meshInstanceAabbs;

    auto axis_sorted_aabbs = new Geometry::AxisSortedAabb[numMeshInstanceAabbs];
    for (int i = 0; i < numMeshInstanceAabbs; i++) {
        axis_sorted_aabbs[i] = aabb_array[i];
    }

    std::vector<Geometry::AxisSortedAabb*> axis_sorted_aabs_ptrs (numMeshInstanceAabbs);
    for (int i = 0; i < numMeshInstanceAabbs; i++) {
        axis_sorted_aabs_ptrs[i] =  &axis_sorted_aabbs[i];
    }

    auto top_node = InitMeshBvh ();

    ConstructBvhForAabbArray (axis_sorted_aabs_ptrs, top_node, InitMeshBvh);

    delete axis_sorted_aabbs;
    return top_node;
    return nullptr;
}

const Geometry::Aabb & Scene::GetMeshInstanceAabb (const MeshInstance* mesh_instance) {
    return meshInstanceAabbs[mesh_instance->aabbOffset];
}



inline Mesh* Scene::InitMesh ( int num_instances ) {
    auto& new_mesh = meshes[numMeshes++];
    new_mesh.vbOffset                = curVbOffset;
    new_mesh.ibOffset                = curIbOffset;
    new_mesh.modelMatrixBufferOffset = curModelMatrixBufferOffset;
    new_mesh.instancesOffset         = curMeshInstancesOffset;
    new_mesh.numInstances            = num_instances;
    new_mesh.facesAabbOffset         = curFaceAabbOffset;

    return &new_mesh;
}

inline void Scene::CalculateFaceAabb (int face_idx) {
    int* face_pointer = ibPointer + 3 * face_idx;
    auto v0 = vbPointer + face_pointer[0];
    auto v1 = vbPointer + face_pointer[1];
    auto v2 = vbPointer + face_pointer[2];    

    auto x_min = min (v0->Position.x, v1->Position.x);
    x_min = min (x_min, v2->Position.x);
    auto y_min = min (v0->Position.y, v1->Position.y);
    y_min = min (y_min, v2->Position.y);
    auto z_min = min (v0->Position.z, v1->Position.z);
    z_min = min (z_min, v2->Position.z);

    auto x_max = max (v0->Position.x, v1->Position.x);
    x_max = max (x_max, v2->Position.x);
    auto y_max = max (v0->Position.y, v1->Position.y);
    y_max = max (y_max, v2->Position.y);
    auto z_max = max (v0->Position.z, v1->Position.z);
    z_max = max (z_max, v2->Position.z);

    auto& face_aabb = faceAabbs[face_idx];
    face_aabb.faceIdx = face_idx;
    face_aabb.Min     = { x_min, y_min, z_min };
    face_aabb.Max     = { x_max, y_max, z_max };
}

inline void Scene::CalculateMeshFacesAabbs (Mesh* mesh) {
    auto faces_start_idx = mesh->ibOffset;
    auto num_faces       = mesh->ibSize / 3;
    auto faces_last_idx  = faces_start_idx + num_faces;

    for (int f = faces_start_idx; f < faces_last_idx; f++) {
        CalculateFaceAabb (f);
    }
}

inline void Scene::CalculateMeshAabb (Mesh * mesh) {
    auto verticex_start_idx = mesh->ibOffset;
    auto num_vertices       = mesh->ibSize;
    auto vertices_last_idx  = verticex_start_idx + num_vertices;

    auto& mesh_aabb = mesh->aabb;

    
    for (int v = verticex_start_idx; v < vertices_last_idx; v++) {
        auto cur_vertex = vbPointer[ibPointer[v]];

        if (cur_vertex.Position.x < mesh_aabb.Min.x) {
            mesh_aabb.Min.x = cur_vertex.Position.x;
        }
        if (cur_vertex.Position.y < mesh_aabb.Min.y) {
            mesh_aabb.Min.y = cur_vertex.Position.y;
        }
        if (cur_vertex.Position.z < mesh_aabb.Min.z) {
            mesh_aabb.Min.z = cur_vertex.Position.z;
        }

        if (cur_vertex.Position.x > mesh_aabb.Max.x) {
            mesh_aabb.Max.x = cur_vertex.Position.x;
        }
        if (cur_vertex.Position.y > mesh_aabb.Max.y) {
            mesh_aabb.Max.y = cur_vertex.Position.y;
        }
        if (cur_vertex.Position.z > mesh_aabb.Max.z) {
            mesh_aabb.Max.z = cur_vertex.Position.z;
        }
    }
}

inline void Scene::ConstructBvhForAabbArray (const std::vector<Geometry::AxisSortedAabb*>& aabbs, 
                                             Geometry::Bvh * bvh, 
                                             Geometry::Bvh* (*init_function)() ) {
    int num_aabbs = aabbs.size ();

    if (num_aabbs <= 2) {
        for (int i = 0; i < num_aabbs; ++i) {
            bvh->faceAabbs[i] = *aabbs[i];
        }
        return;
    }

    std::vector<Geometry::AxisSortedAabb*>
        sort_min_aabbs[3] = { aabbs, aabbs, aabbs };
    std::vector<Geometry::AxisSortedAabb*>
        sort_max_aabbs[3] = { aabbs, aabbs, aabbs };

    SortAabbsByEachAxis (sort_min_aabbs, sort_max_aabbs);


    float min_sah = FLT_MAX;

    Geometry::Aabb best_left  = {};
    Geometry::Aabb best_right = {};


    int best_division_idx = 0;
    int best_axis         = 0;

    for (int i = 0; i < 3; i++) {  // i represents axis (0~x, 1~y, 2~z)
        Geometry::Aabb aabb_left = *sort_max_aabbs[i][0];

        // The volume containing all aabbs  
        Geometry::Aabb aabb_right = { { sort_min_aabbs[0][0]->Min.x,
                                        sort_min_aabbs[1][0]->Min.y,
                                        sort_min_aabbs[2][0]->Min.z },
                                      
                                      { sort_max_aabbs[0][num_aabbs - 1]->Max.x,
                                        sort_max_aabbs[1][num_aabbs - 1]->Max.y,
                                        sort_max_aabbs[2][num_aabbs - 1]->Max.z } };

        int min_idx[3] = { 0, 0, 0 };
        int max_idx[3] = { num_aabbs - 1, num_aabbs - 1, num_aabbs - 1 };


        for (int e = 0; e < num_aabbs; e++) {
            auto cur_aabb =  sort_max_aabbs[i][e];

            // Left Volume must include all of the previously processed aabbs,
            // so we need to adjust its borders.
            for (int a = 0; a < 3; ++a) {
                if (cur_aabb->Min[a] < aabb_left.Min[a]) {
                    aabb_left.Min[a] = cur_aabb->Min[a];
                }
                if (cur_aabb->Max[a] > aabb_left.Max[a]) {
                    aabb_left.Max[a] = cur_aabb->Max[a];
                }
            }

            ShrinkBvhNodeVolume ( i, e, num_aabbs,
                                  sort_min_aabbs, sort_max_aabbs,
                                  min_idx, max_idx,
                                  &aabb_right, cur_aabb );

            float sah = CalculateSah (aabb_left, aabb_right,
                                      e, num_aabbs);

            if (sah < min_sah) {
                best_axis = i;
                best_division_idx = e;
                min_sah = sah;

                best_left  = aabb_left;
                best_right = aabb_right;
            }
        }
    }

    std::vector<Geometry::AxisSortedAabb*> left_node_aabbs;
    std::vector<Geometry::AxisSortedAabb*> right_node_aabbs;

    left_node_aabbs.insert (left_node_aabbs.begin (), sort_max_aabbs[best_axis].begin (), sort_max_aabbs[best_axis].begin () + best_division_idx + 1);
    right_node_aabbs.insert (right_node_aabbs.begin (), sort_max_aabbs[best_axis].begin () + best_division_idx + 1, sort_max_aabbs[best_axis].end ());

    bvh->leftNode = init_function ();
    bvh->leftNode->boundary = best_left;

    bvh->rightNode = init_function ();
    bvh->rightNode->boundary = best_right;

    ConstructBvhForAabbArray (left_node_aabbs, bvh->leftNode, init_function);
    ConstructBvhForAabbArray (right_node_aabbs, bvh->rightNode, init_function);
}

inline void Scene::SortAabbsByEachAxis ( std::vector<Geometry::AxisSortedAabb*>* sort_min_aabbs, 
                                         std::vector<Geometry::AxisSortedAabb*>* sort_max_aabbs ) {
    int n_aabbs = sort_max_aabbs[0].size ();

    for (int i = 0; i < 3; i++) {
        std::sort (sort_min_aabbs[i].begin (), sort_min_aabbs[i].end (),
                   [i](Geometry::AxisSortedAabb* aabb1, Geometry::AxisSortedAabb* aabb2)
        {
            return aabb2->Min[i] > aabb1->Min[i];
        });

        //Assign each aabb its position in every sorted array
        for (int j = 0; j < n_aabbs; ++j) {
            sort_min_aabbs[i][j]->posInMinArray[i] = j;
        }
    }

    for (int i = 0; i < 3; i++) {
        std::sort (sort_max_aabbs[i].begin (), sort_max_aabbs[i].end (),
                   [i](Geometry::AxisSortedAabb* aabb1, Geometry::AxisSortedAabb* aabb2)
        {
            return aabb2->Max[i] > aabb1->Max[i];
        });

        for (int j = 0; j < n_aabbs; ++j) {
            sort_max_aabbs[i][j]->idxInMaxArray[i] = j;
        }
    }
}

inline void Scene::ShrinkBvhNodeVolume ( int axis, int cur_aabb_idx, int num_aabbs,
                                         std::vector<Geometry::AxisSortedAabb*>* sort_min_aabbs,
                                         std::vector<Geometry::AxisSortedAabb*>* sort_max_aabbs,
                                         int * min_idx, int * max_idx,
                                         Geometry::Aabb* aabb_right,
                                         Geometry::AxisSortedAabb* cur_aabb ) {
    // As we move one aabb from left to right, 
    // the right extent also has to recalculate its borders
    // We do it by figuring out if the current aabb has a bigger 
    // shrinked length by each axis than the current min extent.
    // If so we need to adjust the right volume and set the new 
    // index for the min and max aabbs
    auto min_aabb = sort_min_aabbs[axis][min_idx[axis]];
    if (cur_aabb == min_aabb) {
        for (int idx = min_idx[axis]; idx < num_aabbs; idx++) {
            min_aabb = sort_min_aabbs[axis][idx];
            int idx_in_max_array = min_aabb->idxInMaxArray[axis];
            if (idx_in_max_array > cur_aabb_idx) {
                min_idx[axis] = idx;
                aabb_right->Min[axis] = sort_min_aabbs[axis][idx]->Min[axis];
                break;
            }
        }
    }

    for (int a = 1; a < 3; a++) {
        int n = (a + axis) % 3;
        if (cur_aabb->Min[n] == sort_min_aabbs[n][min_idx[n]]->Min[n]) {
            for (int idx = min_idx[n]; idx < num_aabbs; idx++) {
                if (sort_min_aabbs[n][idx]->idxInMaxArray[axis] > cur_aabb_idx) {
                    min_idx[n] = idx;
                    aabb_right->Min[n] = sort_min_aabbs[n][idx]->Min[n];
                    break;
                }
            }
        }
    }

    for (int a = 1; a < 3; a++) {
        int n = (a + axis) % 3;
        if (cur_aabb->Max[n] == sort_max_aabbs[n][max_idx[n]]->Max[n]) {
            for (int idx = max_idx[n]; idx >= 0; idx--) {
                if (sort_max_aabbs[n][idx]->idxInMaxArray[axis] > cur_aabb_idx) {
                    max_idx[n] = idx;
                    aabb_right->Max[n] = sort_max_aabbs[n][idx]->Max[n];
                    break;
                }
            }
        }
    }
}

inline float Scene::CalculateSah ( const Geometry::Aabb & left_aabb, 
                                   const Geometry::Aabb & right_aabb, 
                                   int cur_aabb_idx, int num_aabbs ) {
    float left_side[3];
    for (int i = 0; i < 3; ++i) {
        left_side[i] = left_aabb.Max[i] - left_aabb.Min[i];
    }

    float left_sah = (cur_aabb_idx + 1) * (left_side[0] * left_side[1] +
                                           left_side[1] * left_side[2] +
                                           left_side[2] * left_side[0]);

    float right_side[3];
    for (int i = 0; i < 3; ++i) {
        right_side[i] = right_aabb.Max[i] - right_aabb.Min[i];
    }

    float right_sah = (num_aabbs - cur_aabb_idx - 1) * (right_side[0] * right_side[1] +
                                                        right_side[1] * right_side[2] +
                                                        right_side[2] * right_side[0]);

    return left_sah + right_sah;
}

inline Geometry::Bvh * Scene::InitFaceBvh () {
    return &faceBvhs[numFaceBvh++];
}

inline Geometry::Bvh * Scene::InitMeshBvh () {
    return &meshBvhs[numMeshBvhs++];
}

inline void Scene::InitializePbrShader () {
    pbrShader = DxShell::CreateShader ();
    pbrShader->AssignInputLayot (Geometry::Vertex3d::INPUT_LAYOUT);
    pbrShader->AssignVertexHlsl (L"GraphicsShell/Shaders/Vertex/vs_model.hlsl");
    pbrShader->AssignPixelHlsl  (L"GraphicsShell/Shaders/Pixel/ps_mesh.hlsl");
    pbrShader->SetPrimitiveTopology (D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pbrShader->AddConstantBufferInput (0, D3D12_SHADER_VISIBILITY_VERTEX);
    pbrShader->AddConstantInput (1, 1, D3D12_SHADER_VISIBILITY_VERTEX);
    pbrShader->AddConstantBufferInput (2, D3D12_SHADER_VISIBILITY_VERTEX);
    pbrShader->AddTextureArrayInput (2, 0, D3D12_SHADER_VISIBILITY_PIXEL);

    DxShell::CommitShader (pbrShader);
}

void Scene::UpdateAabbs ( ) {
    
}

Mesh* Scene::CreateSurfaceMesh( float size )
{
    auto scale = 100.0f;
    auto num_patches_x = 255;
    auto num_patches_y = 255;
    auto num_patches = num_patches_x * num_patches_y;

    auto num_vertices_x = num_patches_x + 1;
    auto num_vertices_y = num_patches_y + 1;
    auto num_vertices = num_vertices_x * num_vertices_y;

    auto new_mesh = InitMesh();
    new_mesh->vbSize = num_vertices;
    new_mesh->ibSize = num_patches * 6;

    auto current_vb_ptr = vbPointer + curVbOffset;

    auto vertices = current_vb_ptr;

    float* height_map;
    GenerateHeightMap(&height_map, 256);

    for (auto i = 0; i < num_vertices_x; i++) {
        for (auto j = 0; j < num_vertices_y; j++) {
            auto vertex_idx = j * num_vertices_x + i;
            vertices[vertex_idx].Position = { i * 5.0f, scale * height_map[vertex_idx], j * 5.0f };
        }
    }
    curVbOffset += num_vertices;

    auto indices = ibPointer + curIbOffset;
    for (auto i = 0; i < num_patches_x-1; i++) {
        for (auto j = 0; j < num_patches_y-1; j++) {
            auto patch_idx = j * num_patches_x + i;
            indices[6 * patch_idx + 0] = j * num_vertices_x + i;
            indices[6 * patch_idx + 1] = (j+1) * num_vertices_x + i;
            indices[6 * patch_idx + 2] = j * num_vertices_x + i + 1;
            indices[6 * patch_idx + 3] = j * num_vertices_x + i + 1;
            indices[6 * patch_idx + 4] = (j + 1) * num_vertices_x + i;
            indices[6 * patch_idx + 5] = (j + 1) * num_vertices_x + i + 1;
        }
    }



    
   /* indices[0] = 0;
    indices[1] = 9;
    indices[2] = 1;
    indices[3] = 1;
    indices[4] = 9;
    indices[5] = 10;*/
    curIbOffset += 6;

    new_mesh->instancesOffset = curMeshInstancesOffset;
    curMeshInstancesOffset += DEFAULT_NUM_INSTANCES;

    delete[] height_map;
    return new_mesh;
}


void Scene::GenerateHeightMap(float** height_map, int size) {
    srand(12345);

    // generate initial random map
    auto new_size = 8;
    auto new_map = new float[new_size * new_size];
    auto magnitude = 1.0f;
    for ( auto i = 0; i < new_size; i++ ) {
        for (auto j = 0; j < new_size; j++) {
            auto idx = j * new_size + i;
            new_map[idx] = (float)(rand() % 16384) / 16384.0f - 0.5f;
        }
    }


    while (new_size < size) {
        magnitude /= 1.5f;
        new_size *= 2;
        *height_map = new float[new_size * new_size];

        for (auto i = 0; i < new_size; i++) {
            for (auto j = 0; j < new_size; j++) {
                auto idx = j * new_size + i;
                (*height_map)[idx] = 0.0f;
            }
        }

        // fill the new map with the old points
        for (auto i = 1; i < new_size/2-1; i++) {
            for (auto j = 1; j < new_size/2-1; j++) {
                auto old_idx = j * new_size / 2 + i;
                auto new_idx = 2 * j * new_size + i * 2;
                (*height_map)[new_idx] = new_map[old_idx];
            }
        }


        auto resized_map = new float[new_size * new_size];

        // apply moving average
        for (auto i = 1; i < new_size-1; i++) {
            for (auto j = 1; j < new_size-1; j++) {
                auto average_value = 0.0f;

                for (auto y = 0; y <= 1; y++) {
                    for (auto x = 0; x <= 1; x++) {
                        auto idx = (j + y) * new_size + i + x;
                        average_value += (*height_map)[idx];
                    }
                }

                resized_map[j * new_size + i] = average_value;
            }
        }

        for (auto i = 1; i < new_size-1; i++) {
            for (auto j = 1; j < new_size-1; j++) {
                auto idx = j * new_size + i;
                (*height_map)[idx] = resized_map[idx];
            }
        }

        delete[] resized_map;

        // add new random value to the map
        for (auto i = 1; i < new_size - 1; i++) {
            for (auto j = 1; j < new_size - 1; j++) {
                auto idx = j * new_size + i;
                (*height_map)[idx] += magnitude * (float)(rand() % 16384) / 16384.0f - magnitude/2;
            }
        }


        delete[] new_map;
        new_map = *height_map;
    }
}
