#include "GraphicsShell.h"

#include "dds.h"

#include "Geometry/Scene/Scene.h"
#include "Geometry/Scene/Mesh.h"

void GraphicsShell::Initialize ( HWND window_handle ) {
    windowHandle = window_handle;
    DxShell::Initialize ( windowHandle );

    RECT wnd_rect;
    if ( !GetClientRect ( window_handle, &wnd_rect ) )
        return;

    int screenWidth  = wnd_rect.right - wnd_rect.left;
    int screenHeight = wnd_rect.bottom - wnd_rect.top;

    screen[0] = screenWidth;
    screen[1] = screenHeight;
    aspect = (float)screenWidth / screenHeight;

    mousePosition = { screen[0] / 2, screen[1] / 2 };
    ClientToScreen ( windowHandle, &mousePosition );
    SetCursorPos ( mousePosition.x, mousePosition.y );
    ShowCursor ( FALSE );
    mousePosition = { screen[0] / 2, screen[1] / 2 };

    Scene::Initialize ( );

    InitUnitPlane ( );
    InitDebugGeometryBuffers ();
    

    // -- Shaders --
    InitUnitPlaneShader ( );
    InitMeshShader ( );
    InitLineShader ();

    //
    InitTextures ( );
    InitCamera ( );

    positionBuffer = DxShell::CreateBuffer ( 20 * sizeof ( int ), DxBuffer::Type::CPU_BUFFER );
    auto geometry = reinterpret_cast<int*>(positionBuffer->GetDataPointer ( ));
    geometry[0] = 500;
    geometry[1] = 100;
    geometry[2] = 200;
    geometry[3] = 100;
    geometry[4] = 600;
    geometry[5] = 400;
    geometry[6] = 100;
    geometry[7] = 100;


    FillScene ();



    /*auto box_mesh = LoadObj ( "Mesh/box.vb" );
    auto box_instance_0 = MeshBuffer::CreateMeshInstance ( box_mesh );
    box_instance_0->Translate ( 0.0f, 0.0f, 200.0f );
    box_instance_0->UpdateModelMatrix ( );
    auto box_instance_1 = MeshBuffer::CreateMeshInstance ( box_mesh );
    //box_instance_1->Translate ( 0.0f, 0.0f, 1000.0f );
    //box_instance_1->UpdateModelMatrix ( );   */

    


    /*auto sphere_mesh = Scene::CreateMeshFromObj("Mesh/sphere.vb");
    auto sphere_instance_0 = Scene::CreateMeshInstance ( sphere_mesh );
    sphere_instance_0->SetPosition ( 0.0f, 0.0f, 0.0f );
    sphere_instance_0->UpdateModelMatrix ( );

    auto bvh = Scene::ConstructBvhForMesh (sphere_mesh);
    AddBvhTreeToDebugGeometry (bvh, 0, 2);   */
}

void GraphicsShell::DeInitialize ( ) {
    Scene::DeInitialize ( );
}



void GraphicsShell::Render ( ) {
    DxShell::ClearFrameBuffer ( { 0.15f, 0.12f, 0.2f, 1.0f } );

    DxShell::BindShader ( texturePlaneShader );
    DxShell::BindConstants ( 0, 2, &screen, 0 );
    DxShell::BindCBV ( 1, positionBuffer );
    DxShell::BindTextureArray ( 2, texture );

    DxShell::BindVertexBuffer ( uPlaneVertexBuffer );
    DxShell::BindIndexBuffer ( uPlaneIndexBuffer );

    DxShell::DrawIndexed ( 6, 2, 0, 0 );

    //Scene::RenderForCamera (&mainCamera);

    int color = 255;

    DxShell::BindShader (lineShader);
    DxShell::BindCBV (0, mainCamera.GetViewMatrixBuffer());
    DxShell::BindVertexBuffer (debugGeometryVertexBuffer);
    DxShell::Draw (numDebugLines * 2, 1, 0);
    


    DxShell::SetSwapChainToPresentState ( );
    DxShell::Execute ( );
    DxShell::ShowCurrentFrame ( );
}

DxBuffer* GraphicsShell::LoadDds ( std::string filename, int* width, int* height ) {
    char                dwMagic[4];
    DDS_HEADER          header;

    std::ifstream dds_file ( filename, std::ios::binary );
    dds_file.read ( reinterpret_cast<char*>(dwMagic), sizeof ( DWORD ) );
    dds_file.read ( reinterpret_cast<char*>(&header), sizeof ( DDS_HEADER ) );

    int buffer_size = header.ddspf.dwSize * header.dwWidth * header.dwHeight / 8;
    auto tx_buffer = DxShell::CreateBuffer ( buffer_size, DxBuffer::Type::CPU_BUFFER );
    auto buffer_ptr = reinterpret_cast<char*>(tx_buffer->GetDataPointer ( ));


    dds_file.read ( buffer_ptr, buffer_size );
    if ( dds_file.fail() ) {
        return nullptr;
    }
    *width = header.dwWidth;
    *height = header.dwHeight;
    return tx_buffer;
}

void GraphicsShell::InitUnitPlane ( ) {
    auto temp_buffer = DxShell::CreateBuffer ( 4 * sizeof ( Geometry::Vertex2d ) + 6 * sizeof ( int ), DxBuffer::Type::CPU_BUFFER );
    auto temp_buffer_pointer =  reinterpret_cast<BYTE*>(temp_buffer->GetDataPointer ( ));
    auto vertices = reinterpret_cast<Geometry::Vertex2d*>(temp_buffer_pointer);
    vertices[0] = { { 0.0f,  0.0f }, {0.0f, 0.0f} };
    vertices[1] = { { 0.0f, -1.0f }, {0.0f, 1.0f} };
    vertices[2] = { { 1.0f, -1.0f }, {1.0f, 1.0f} };
    vertices[3] = { { 1.0f,  0.0f }, {1.0f, 0.0f} };

    auto indices = reinterpret_cast<int*>(temp_buffer_pointer + 4 * sizeof ( Geometry::Vertex2d ));
    indices[0] = 0;
    indices[1] = 3;
    indices[2] = 1;
    indices[3] = 3;
    indices[4] = 2;
    indices[5] = 1;

    uPlaneVertexBuffer = DxShell::CreateBuffer ( 4 * sizeof ( Geometry::Vertex2d ), DxBuffer::Type::GPU_BUFFER );
    uPlaneIndexBuffer  = DxShell::CreateBuffer ( 6 * sizeof ( int ), DxBuffer::Type::GPU_BUFFER );
    DxShell::CopyBuffer ( uPlaneVertexBuffer, temp_buffer, 4 * sizeof ( Geometry::Vertex2d ), 0, 0 );
    DxShell::CopyBuffer ( uPlaneIndexBuffer, temp_buffer, 6 * sizeof ( int ), 0, 4 * sizeof ( Geometry::Vertex2d ) );

    uPlaneVertexBuffer->SetAsVertexBuffer ( sizeof ( Geometry::Vertex2d ) );
    uPlaneIndexBuffer->SetAsIndexBuffer ( );
}

void GraphicsShell::InitCamera ( ) {
    mainCamera = Camera ( { 0.0f, 0.0f, -10.0f },
                          { 0.0f, 0.0f, 1.0f },
                          { 0.0f, 1.0f, 0.0f }, 
                          3.14f / 3, aspect, 0.1f, 10000.0f );
    mainCamera.RotateXYAbsolute ( 0.0f, 0.0f );    
}

void GraphicsShell::InitUnitPlaneShader ( ) {
    texturePlaneShader = DxShell::CreateShader ( );
    texturePlaneShader->AssignInputLayot ( Geometry::Vertex2d::INPUT_LAYOUT );
    texturePlaneShader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/vs_instanced.hlsl" );
    texturePlaneShader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/ps_plane.hlsl" );
    texturePlaneShader->AddConstantInput ( 2, 0, D3D12_SHADER_VISIBILITY_VERTEX );
    texturePlaneShader->AddConstantBufferInput ( 1, D3D12_SHADER_VISIBILITY_VERTEX );
    texturePlaneShader->AddTextureArrayInput ( 2, 0, D3D12_SHADER_VISIBILITY_PIXEL );
    
    DxShell::CommitShader ( texturePlaneShader );
}

void GraphicsShell::InitMeshShader ( ) {
    meshShader = DxShell::CreateShader ( );
    meshShader->AssignInputLayot ( Geometry::Vertex3d::INPUT_LAYOUT );
    meshShader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/vs_model.hlsl" );
    meshShader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/ps_mesh.hlsl" );
    meshShader->SetPrimitiveTopology (D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    meshShader->AddConstantBufferInput ( 0, D3D12_SHADER_VISIBILITY_VERTEX );
    meshShader->AddConstantInput ( 1, 1, D3D12_SHADER_VISIBILITY_VERTEX );
    meshShader->AddConstantBufferInput ( 2, D3D12_SHADER_VISIBILITY_VERTEX );
    meshShader->AddTextureArrayInput ( 2, 0, D3D12_SHADER_VISIBILITY_PIXEL );

    DxShell::CommitShader ( meshShader );
}

void GraphicsShell::InitLineShader ()
{
    lineShader = DxShell::CreateShader ();
    lineShader->AssignInputLayot (Geometry::Vertex3dPosCol::INPUT_LAYOUT);
    lineShader->AssignVertexHlsl (L"GraphicsShell/Shaders/Vertex/vs_line.hlsl");
    lineShader->AssignPixelHlsl (L"GraphicsShell/Shaders/Pixel/ps_line.hlsl");
    lineShader->SetPrimitiveTopology (D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    lineShader->AddConstantBufferInput (0, D3D12_SHADER_VISIBILITY_VERTEX);
    lineShader->AddConstantInput (1, 1, D3D12_SHADER_VISIBILITY_VERTEX);

    DxShell::CommitShader (lineShader);
}

void GraphicsShell::InitTextures ( ) {
    int width;
    int height;
    auto texture_buffer = GraphicsShell::LoadDds ( "Bitmaps/1.dds", &width, &height );
    texture = DxShell::CreateTexture ( width, height, 1, DXGI_FORMAT_B8G8R8A8_UNORM );
    DxShell::CopyTexture ( texture, texture_buffer );
    DxShell::CreateDescriptorForTexture ( texture, 0 );
    DxShell::SetTextureAsPixelShaderInput ( texture );
    texture_buffer = GraphicsShell::LoadDds ( "Bitmaps/2.dds", &width, &height );
    auto texture2 = DxShell::CreateTexture ( width, height, 1, DXGI_FORMAT_B8G8R8A8_UNORM );
    DxShell::CopyTexture ( texture2, texture_buffer );
    DxShell::CreateDescriptorForTexture ( texture2, 1 );
    DxShell::SetTextureAsPixelShaderInput ( texture2 );
}

void GraphicsShell::InitDebugGeometryBuffers () {
    auto vb_buffer_size_in_bytes = DEBUG_GEOMETRY_BUFFER_SIZE * sizeof (Geometry::Vertex3dPosCol);
    debugGeometryVertexBuffer = DxShell::CreateBuffer (vb_buffer_size_in_bytes, DxBuffer::Type::CPU_BUFFER);
    debugGeometryVBPtr = reinterpret_cast<Geometry::Vertex3dPosCol*> (debugGeometryVertexBuffer->GetDataPointer());

    debugGeometryVertexBuffer->SetAsVertexBuffer (sizeof (Geometry::Vertex3dPosCol));
}

void GraphicsShell::AddAabbToDebugGeometry (const Geometry::Aabb& aabb, DirectX::XMFLOAT3 color) {
    auto vb_pointer = debugGeometryVBPtr;

    vb_pointer[0].Position  = { aabb.Min.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[1].Position  = { aabb.Max.x, aabb.Min.y, aabb.Min.z };

    vb_pointer[2].Position  = { aabb.Min.x, aabb.Min.y, aabb.Max.z };
    vb_pointer[3].Position  = { aabb.Max.x, aabb.Min.y, aabb.Max.z };

    vb_pointer[4].Position  = { aabb.Min.x, aabb.Max.y, aabb.Min.z };
    vb_pointer[5].Position  = { aabb.Max.x, aabb.Max.y, aabb.Min.z };

    vb_pointer[6].Position  = { aabb.Min.x, aabb.Max.y, aabb.Max.z };
    vb_pointer[7].Position  = { aabb.Max.x, aabb.Max.y, aabb.Max.z };



    vb_pointer[8].Position  = { aabb.Min.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[9].Position  = { aabb.Min.x, aabb.Max.y, aabb.Min.z };

    vb_pointer[10].Position = { aabb.Min.x, aabb.Min.y, aabb.Max.z };
    vb_pointer[11].Position = { aabb.Min.x, aabb.Max.y, aabb.Max.z };

    vb_pointer[12].Position = { aabb.Max.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[13].Position = { aabb.Max.x, aabb.Max.y, aabb.Min.z };

    vb_pointer[14].Position = { aabb.Max.x, aabb.Min.y, aabb.Max.z };
    vb_pointer[15].Position = { aabb.Max.x, aabb.Max.y, aabb.Max.z };



    vb_pointer[16].Position = { aabb.Min.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[17].Position = { aabb.Min.x, aabb.Min.y, aabb.Max.z };

    vb_pointer[18].Position = { aabb.Max.x, aabb.Min.y, aabb.Min.z };
    vb_pointer[19].Position = { aabb.Max.x, aabb.Min.y, aabb.Max.z };

    vb_pointer[20].Position = { aabb.Min.x, aabb.Max.y, aabb.Min.z };
    vb_pointer[21].Position = { aabb.Min.x, aabb.Max.y, aabb.Max.z };

    vb_pointer[22].Position = { aabb.Max.x, aabb.Max.y, aabb.Min.z };
    vb_pointer[23].Position = { aabb.Max.x, aabb.Max.y, aabb.Max.z };

    for (int i = 0; i < 24; i++) {
        vb_pointer[i].Color = color;
    }

    debugGeometryVBPtr += 24;
    numDebugLines += 12;
}

void GraphicsShell::AddBvhTreeToDebugGeometry (const Geometry::Bvh* bvh, 
                                               int min_depth, int max_depth, 
                                               DirectX::XMFLOAT3 color) {
    if (max_depth < 0) {
        return;
    }
    auto vb_pointer = debugGeometryVBPtr;

    if (min_depth <= 0) {
        AddAabbToDebugGeometry (bvh->GetBoundary (), color);
    }

    auto left_node = bvh->GetLeftNode ();
    auto right_node = bvh->GetRightNode ();

    if (left_node) {
        AddBvhTreeToDebugGeometry (left_node, min_depth - 1, max_depth - 1, { 0.0f, 1.0f, 0.0f });
    }

    if (right_node) {
        AddBvhTreeToDebugGeometry (right_node, min_depth - 1, max_depth - 1, { 1.0f, 0.0f, 0.0f });
    }
}
 


/*void GraphicsShell::InitMesh ( ) {
    std::string filename = "Mesh/box.vb";
    std::ifstream mesh_fs ( filename, std::ios::binary );
    if ( mesh_fs.fail ( ) ) {
        return;
    }

    

    mesh_fs.read ( reinterpret_cast<char*>(&mesh_size), 4 );

    auto temp_buffer = DxShell::CreateBuffer ( mesh_size * sizeof ( Geometry::Vertex3d ) + mesh_size * sizeof ( int ), DxBuffer::Type::CPU_BUFFER );
    auto temp_buffer_pointer =  reinterpret_cast<BYTE*>(temp_buffer->GetDataPointer ( ));
    auto vertices = reinterpret_cast<Geometry::Vertex3d*>(temp_buffer_pointer);

    mesh_fs.read ( reinterpret_cast<char*>(vertices), mesh_size * sizeof ( Geometry::Vertex3d ) );

    auto indices  = reinterpret_cast<int*>(temp_buffer_pointer + mesh_size * sizeof ( Geometry::Vertex3d ));
    for ( int i = 0; i < mesh_size; i++ ) {
        indices[i] = i;
    }


    meshVertexBuffer = DxShell::CreateBuffer ( mesh_size * sizeof ( Geometry::Vertex3d ), DxBuffer::Type::GPU_BUFFER );
    meshIndexBuffer  = DxShell::CreateBuffer ( mesh_size * sizeof ( int ), DxBuffer::Type::GPU_BUFFER );
    DxShell::CopyBuffer ( meshVertexBuffer, temp_buffer, mesh_size * sizeof ( Geometry::Vertex3d ), 0, 0 );
    DxShell::CopyBuffer ( meshIndexBuffer, temp_buffer, mesh_size * sizeof ( int ), 0, mesh_size * sizeof ( Geometry::Vertex3d ) );

    meshVertexBuffer->SetAsVertexBuffer ( sizeof ( Geometry::Vertex3d ) );
    meshIndexBuffer->SetAsIndexBuffer ( );
}*/

void GraphicsShell::ConstructBvh () {
    /*auto bvh = Geometry::Bvh::CreateFromAabbArray (MeshBuffer::faceAabbs, MeshBuffer::numAabbs);
    AddBvhTreeToDebugGeometry (*bvh, 0, 4, { 0.0f, 0.0f, 1.0f });*/
}

void GraphicsShell::FillScene () {
    auto box_mesh = Scene::CreateMeshFromObj ("Mesh/box.vb");
    auto box_bvh = Scene::ConstructBvhForMesh (box_mesh);

    int num_box_instances = 10;

    for (int i = 0; i < num_box_instances; i++) {
        auto box_instance = Scene::CreateMeshInstance (box_mesh);

        auto position_x = (float)(rand () % 1000);
        auto position_z = (float)(rand () % 1000);

        box_instance->SetPosition (position_x, 0.0f, position_z);
        box_instance->UpdateModelMatrix ();

        //AddAabbToDebugGeometry (Scene::GetMeshInstanceAabb (box_instance),
        //                        { 0.0f,0.0f,1.0f });
    }

    auto sphere_mesh = Scene::CreateMeshFromObj ("Mesh/Sphere.vb");
    auto sphere_bvh = Scene::ConstructBvhForMesh (sphere_mesh);
    int num_sphere_instances = 10;

    for (int i = 0; i < num_sphere_instances; i++) {
        auto sphere_instance = Scene::CreateMeshInstance (sphere_mesh);

        auto position_x = (float)(rand () % 1000);
        auto position_z = (float)(rand () % 1000);

        sphere_instance->SetPosition (position_x, 0.0f, position_z);
        sphere_instance->UpdateModelMatrix ();
        //AddAabbToDebugGeometry (Scene::GetMeshInstanceAabb(sphere_instance),
        //                        { 0.0f,0.0f,1.0f });
    }

    auto scene_bvh = Scene::ConstructSceneBvh ();
    AddBvhTreeToDebugGeometry (scene_bvh, 2, 3);
}
