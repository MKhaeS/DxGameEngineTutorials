#include "GraphicsShell.h"

#include "dds.h"

void GraphicsShell::Initialize ( HWND window_handle ) {
    DxShell::Initialize ( window_handle );

    RECT wnd_rect;
    if ( !GetClientRect ( window_handle, &wnd_rect ) )
        return;

    int screenWidth  = wnd_rect.right - wnd_rect.left;
    int screenHeight = wnd_rect.bottom - wnd_rect.top;

    screen[0] = screenWidth;
    screen[1] = screenHeight;
    size[0] = 100;
    size[1] = 100;

    InitUnitPlane ( );
    InitUnitPlaneShader ( );

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

void GraphicsShell::Render ( ) {

    DxShell::ClearFrameBuffer ( { 0.9f, 0.8f, 0.1f, 1.0f } );
    DxShell::BindShader ( shader );
    DxShell::BindConstants ( 0, 2, &screen, 0 );
    DxShell::BindCBV ( 1, positionBuffer );
    DxShell::BindTextureArray ( 2, texture );

    DxShell::BindVertexBuffer ( vertexBuffer );
    DxShell::BindIndexBuffer ( indexBuffer );

    DxShell::Draw ( 6, 2 );

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

    vertexBuffer = DxShell::CreateBuffer ( 4 * sizeof ( Vertex2d ), DxBuffer::Type::GPU_BUFFER );
    indexBuffer  = DxShell::CreateBuffer ( 6 * sizeof ( int ), DxBuffer::Type::GPU_BUFFER );
    DxShell::CopyBuffer ( vertexBuffer, temp_buffer, 4 * sizeof ( Vertex2d ), 0, 0 );
    DxShell::CopyBuffer ( indexBuffer, temp_buffer, 6 * sizeof ( int ), 0, 4 * sizeof ( Vertex2d ) );

    vertexBuffer->SetAsVertexBuffer ( sizeof ( Vertex2d ) );
    indexBuffer->SetAsIndexBuffer ( );
}

void GraphicsShell::InitUnitPlaneShader ( ) {
    shader = DxShell::CreateShader ( );
    shader->AssignInputLayot ( Vertex2d::INPUT_LAYOUT );
    shader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/vs_instanced.hlsl" );
    shader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/ps.hlsl" );
    shader->AddConstantInput ( 2, 0, D3D12_SHADER_VISIBILITY_VERTEX );
    shader->AddConstantBufferInput ( 1, D3D12_SHADER_VISIBILITY_VERTEX );
    shader->AddTextureArrayInput ( 2, 0, D3D12_SHADER_VISIBILITY_PIXEL );
    
    DxShell::CommitShader ( shader );
}
