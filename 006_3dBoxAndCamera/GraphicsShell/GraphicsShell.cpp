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

}



void GraphicsShell::Render ( ) {

    DxShell::ClearFrameBuffer ( { 0.15f, 0.12f, 0.2f, 1.0f } );
    /*DxShell::BindShader ( texturePlaneShader );
    DxShell::BindConstants ( 0, 2, &screen, 0 );
    DxShell::BindCBV ( 1, positionBuffer );
    DxShell::BindTextureArray ( 2, texture );

    DxShell::BindVertexBuffer ( uPlaneVertexBuffer );
    DxShell::BindIndexBuffer ( uPlaneIndexBuffer );

    DxShell::Draw ( 6, 2 );*/

    DxShell::BindShader ( meshShader );
    DxShell::BindCBV ( 0, viewMatrixBuffer );
    DxShell::BindTextureArray ( 1, texture );

    DxShell::BindVertexBuffer ( meshVertexBuffer );
    DxShell::BindIndexBuffer ( meshIndexBuffer );

    DxShell::Draw ( 36, 1 );

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
    auto temp_buffer = DxShell::CreateBuffer ( 4 * sizeof ( Vertex2d ) + 6 * sizeof ( int ), DxBuffer::Type::CPU_BUFFER );
    auto temp_buffer_pointer =  reinterpret_cast<BYTE*>(temp_buffer->GetDataPointer ( ));
    auto vertices = reinterpret_cast<Vertex2d*>(temp_buffer_pointer);
    vertices[0] = { { 0.0f,  0.0f }, {0.0f, 0.0f} };
    vertices[1] = { { 0.0f, -1.0f }, {0.0f, 1.0f} };
    vertices[2] = { { 1.0f, -1.0f }, {1.0f, 1.0f} };
    vertices[3] = { { 1.0f,  0.0f }, {1.0f, 0.0f} };

    auto indices = reinterpret_cast<int*>(temp_buffer_pointer + 4 * sizeof ( Vertex2d ));
    indices[0] = 0;
    indices[1] = 3;
    indices[2] = 1;
    indices[3] = 3;
    indices[4] = 2;
    indices[5] = 1;

    uPlaneVertexBuffer = DxShell::CreateBuffer ( 4 * sizeof ( Vertex2d ), DxBuffer::Type::GPU_BUFFER );
    uPlaneIndexBuffer  = DxShell::CreateBuffer ( 6 * sizeof ( int ), DxBuffer::Type::GPU_BUFFER );
    DxShell::CopyBuffer ( uPlaneVertexBuffer, temp_buffer, 4 * sizeof ( Vertex2d ), 0, 0 );
    DxShell::CopyBuffer ( uPlaneIndexBuffer, temp_buffer, 6 * sizeof ( int ), 0, 4 * sizeof ( Vertex2d ) );

    uPlaneVertexBuffer->SetAsVertexBuffer ( sizeof ( Vertex2d ) );
    uPlaneIndexBuffer->SetAsIndexBuffer ( );
}

void GraphicsShell::InitMesh ( ) {
    auto temp_buffer = DxShell::CreateBuffer ( 8 * sizeof ( Vertex3d ) + 36 * sizeof ( int ), DxBuffer::Type::CPU_BUFFER );
    auto temp_buffer_pointer =  reinterpret_cast<BYTE*>(temp_buffer->GetDataPointer ( ));
    auto vertices = reinterpret_cast<Vertex3d*>(temp_buffer_pointer);

    float side = 1.0f;
    vertices[0] = { { -side, -side, -side }, {0.0f, 0.0f} };
    vertices[1] = { {  side, -side, -side }, {1.0f, 0.0f} };
    vertices[2] = { { -side,  side, -side }, {0.0f, 1.0f} };
    vertices[3] = { {  side,  side, -side }, {1.0f, 1.0f} };
    vertices[4] = { { -side, -side,  side }, {0.0f, 0.0f} };
    vertices[5] = { {  side, -side,  side }, {1.0f, 0.0f} };
    vertices[6] = { { -side,  side,  side }, {0.0f, 1.0f} };
    vertices[7] = { {  side,  side,  side }, {1.0f, 1.0f} };

    auto indices = reinterpret_cast<DirectX::XMINT3*>(temp_buffer_pointer + 8 * sizeof ( Vertex3d ));
    indices[0]  = { 0, 3, 1 };
    indices[1]  = { 3, 0, 2 };

    indices[2]  = { 5, 6, 4 };
    indices[3]  = { 6, 5, 7 };

    indices[4]  = { 4, 2, 0 };
    indices[5]  = { 2, 4, 6 };

    indices[6]  = { 1, 7, 5 };
    indices[7]  = { 7, 1, 3 };

    indices[8]  = { 2, 7, 3 };
    indices[9]  = { 7, 2, 6 };

    indices[10] = { 1, 4, 0 };
    indices[11] = { 4, 1, 5 };

    meshVertexBuffer = DxShell::CreateBuffer ( 8 * sizeof ( Vertex3d ), DxBuffer::Type::GPU_BUFFER );
    meshIndexBuffer  = DxShell::CreateBuffer ( 36 * sizeof ( int ), DxBuffer::Type::GPU_BUFFER );
    DxShell::CopyBuffer ( meshVertexBuffer, temp_buffer, 8 * sizeof ( Vertex3d ), 0, 0 );
    DxShell::CopyBuffer ( meshIndexBuffer, temp_buffer, 36 * sizeof ( int ), 0, 8 * sizeof ( Vertex3d ) );

    meshVertexBuffer->SetAsVertexBuffer ( sizeof ( Vertex3d ) );
    meshIndexBuffer->SetAsIndexBuffer ( );
}

void GraphicsShell::InitCamera ( ) {
    mainCamera = Camera ( { 0.0f, 0.0f, -10.0f },
                          { 0.0f, 0.0f, 1.0f },
                          { 0.0f, 1.0f, 0.0f }, 
                          3.14f / 3, aspect, 0.1f, 1000.0f );
    mainCamera.RotateXYAbsolute ( 0.0f, 0.0f );

    viewMatrixBuffer = DxShell::CreateBuffer ( sizeof ( DirectX::XMFLOAT4X4A ), DxBuffer::Type::CPU_BUFFER );
    viewMatrixPtr = reinterpret_cast<DirectX::XMFLOAT4X4*>(viewMatrixBuffer->GetDataPointer ( ));

    (*viewMatrixPtr) = mainCamera.GetViewMatrix ( );
}

void GraphicsShell::InitUnitPlaneShader ( ) {
    texturePlaneShader = DxShell::CreateShader ( );
    texturePlaneShader->AssignInputLayot ( Vertex2d::INPUT_LAYOUT );
    texturePlaneShader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/vs_instanced.hlsl" );
    texturePlaneShader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/ps.hlsl" );
    texturePlaneShader->AddConstantInput ( 2, 0, D3D12_SHADER_VISIBILITY_VERTEX );
    texturePlaneShader->AddConstantBufferInput ( 1, D3D12_SHADER_VISIBILITY_VERTEX );
    texturePlaneShader->AddTextureArrayInput ( 2, 0, D3D12_SHADER_VISIBILITY_PIXEL );
    
    DxShell::CommitShader ( texturePlaneShader );
}

void GraphicsShell::InitMeshShader ( ) {
    meshShader = DxShell::CreateShader ( );
    meshShader->AssignInputLayot ( Vertex3d::INPUT_LAYOUT );
    meshShader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/vs_model.hlsl" );
    meshShader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/ps.hlsl" );
    meshShader->AddConstantBufferInput ( 0, D3D12_SHADER_VISIBILITY_VERTEX );
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
