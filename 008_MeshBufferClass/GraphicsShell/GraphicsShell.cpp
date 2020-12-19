#include "GraphicsShell.h"

#include "dds.h"

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

    MeshBuffer::Initialize ( );
    currentVBPointer = MeshBuffer::vbPointer;
    currentIBPointer = MeshBuffer::ibPointer;

    InitUnitPlane ( );
    InitMesh ( );
    InitUnitPlaneShader ( );
    InitMeshShader ( );
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



    auto box_mesh = LoadObj ( "Mesh/box.vb" );
    auto box_instance_0 = MeshBuffer::CreateMeshInstance ( box_mesh );
    box_instance_0->Translate ( 0.0f, 0.0f, 200.0f );
    box_instance_0->UpdateModelMatrix ( );
    auto box_instance_1 = MeshBuffer::CreateMeshInstance ( box_mesh );
    //box_instance_1->Translate ( 0.0f, 0.0f, 1000.0f );
    //box_instance_1->UpdateModelMatrix ( );

    auto sphere_mesh = LoadObj ( "Mesh/sphere.vb" );
    auto sphere_instance_0 = MeshBuffer::CreateMeshInstance ( sphere_mesh );
    sphere_instance_0->Translate ( 200.0f, 0.0f, 200.0f );
    sphere_instance_0->UpdateModelMatrix ( );

}

void GraphicsShell::DeInitialize ( ) {
    MeshBuffer::DeInitialize ( );
}

void GraphicsShell::Render ( ) {

    DxShell::ClearFrameBuffer ( { 0.15f, 0.12f, 0.2f, 1.0f } );
    DxShell::BindShader ( texturePlaneShader );
    DxShell::BindConstants ( 0, 2, &screen, 0 );
    DxShell::BindCBV ( 1, positionBuffer );
    DxShell::BindTextureArray ( 2, texture );

    DxShell::BindVertexBuffer ( uPlaneVertexBuffer );
    DxShell::BindIndexBuffer ( uPlaneIndexBuffer );

    DxShell::Draw ( 6, 2, 0, 0 );

    DxShell::BindShader ( meshShader );
    DxShell::BindCBV ( 0, viewMatrixBuffer );
    DxShell::BindCBV ( 2, MeshBuffer::modelMatrixBuffer );
    DxShell::BindTextureArray ( 3, texture );

    DxShell::BindVertexBuffer ( MeshBuffer::vertexBuffer );
    DxShell::BindIndexBuffer  ( MeshBuffer::indexBuffer );



    for ( int m = 0; m < MeshBuffer::numMeshes; m++ ) {
        DxShell::BindConstants ( 1, 1, &MeshBuffer::meshes[m].modelMatrixBufferOffset, 0 );
        DxShell::Draw ( MeshBuffer::meshes[m].ibSize,
                        MeshBuffer::meshes[m].numInstances,
                        MeshBuffer::meshes[m].ibOffset,
                        MeshBuffer::meshes[m].vbOffset
                        );
    }
    


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

Mesh * GraphicsShell::LoadObj ( std::string filename ) {
    std::ifstream mesh_fs ( filename, std::ios::binary );
    if ( mesh_fs.fail ( ) ) {
        return nullptr;
    }

    auto new_mesh = MeshBuffer::CreateMesh ( );

    int size = 0;
    mesh_fs.read ( reinterpret_cast<char*>(&size), 4 );
    new_mesh->vbSize = size;
    new_mesh->ibSize = size;
    new_mesh->vbOffset = currentVBOffset;
    new_mesh->ibOffset = currentIBOffset;


    mesh_fs.read ( reinterpret_cast<char*>(currentVBPointer), size * sizeof ( Geometry::Vertex3d ) );

    auto indices  = currentIBPointer;
    for ( int i = 0; i < size; i++ ) {
        indices[i] = i;
    }
    
    int num_faces = size / 3;
    

    for ( int f = 0; f < num_faces; f++ ) {
        int* face_pointer = currentIBPointer + 3 * f;
        auto v0 = currentVBPointer + face_pointer[0];
        auto v1 = currentVBPointer + face_pointer[1];
        auto v2 = currentVBPointer + face_pointer[2];

        auto face_aabb =  MeshBuffer::CreateFaceAabb ( );

        auto x_min = min ( v0->Position.x, v1->Position.x );
        x_min = min ( x_min, v2->Position.x );
        auto y_min = min ( v0->Position.y, v1->Position.y );
        y_min = min ( y_min, v2->Position.y );
        auto z_min = min ( v0->Position.z, v1->Position.z );
        z_min = min ( z_min, v2->Position.z );

        auto x_max = max ( v0->Position.x, v1->Position.x );
        x_max = max ( x_max, v2->Position.x );
        auto y_max = max ( v0->Position.y, v1->Position.y );
        y_max = max ( y_max, v2->Position.y );
        auto z_max = max ( v0->Position.z, v1->Position.z );
        z_max = max ( z_max, v2->Position.z );

        face_aabb->Min = { x_min, y_min, z_min };
        face_aabb->Max = { x_max, y_max, z_max };

        face_aabb->faceOffset = currentIBOffset + f;
    }

    currentVBPointer += size;
    currentIBPointer += size;
    currentVBOffset  += size;
    currentIBOffset  += size;

    return new_mesh;
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

    viewMatrixBuffer = DxShell::CreateBuffer ( sizeof ( DirectX::XMFLOAT4X4A ), DxBuffer::Type::CPU_BUFFER );
    viewMatrixPtr = reinterpret_cast<DirectX::XMFLOAT4X4*>(viewMatrixBuffer->GetDataPointer ( ));

    (*viewMatrixPtr) = mainCamera.GetViewMatrix ( );
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
    meshShader->AddConstantBufferInput ( 0, D3D12_SHADER_VISIBILITY_VERTEX );
    meshShader->AddConstantInput ( 1, 1, D3D12_SHADER_VISIBILITY_VERTEX );
    meshShader->AddConstantBufferInput ( 2, D3D12_SHADER_VISIBILITY_VERTEX );
    meshShader->AddTextureArrayInput ( 2, 0, D3D12_SHADER_VISIBILITY_PIXEL );

    DxShell::CommitShader ( meshShader );
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
 
void GraphicsShell::InitMesh ( ) {
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
}
