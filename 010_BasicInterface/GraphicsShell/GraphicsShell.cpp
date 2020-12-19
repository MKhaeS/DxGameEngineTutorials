#include "GraphicsShell.h"

#include "Geometry/Scene/Scene.h"
#include "Geometry/Scene/Mesh.h"

void GraphicsShell::Initialize ( HWND window_handle ) {
    windowHandle = window_handle;
    DxShell::Initialize ( windowHandle );

    RECT wnd_rect;
    if ( !GetClientRect ( window_handle, &wnd_rect ) ) { return; }

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

    Scene::Initialize (  );
    Gui::Initialize ( window_handle );    

    InitDebugGeometryBuffers ();
    InitGui ( );

    // -- Shaders --
    InitMeshShader ( );
    InitLineShader ();

    //
    InitTextures ( );
    InitCamera ( );

    AddAxisToDebugGeometry ( );

}

void GraphicsShell::DeInitialize ( ) {
    Scene::DeInitialize ( );
}

const LightSource * GraphicsShell::CreateLightSource ( DirectX::XMFLOAT3 position, float intencity ) {
    auto new_light_source = Scene::CreateLightSource ( position, intencity );
    AddLightSourceToDebugGeometry ( *new_light_source );
    return new_light_source;
}

void GraphicsShell::UpdateAabbs ( ) {
    
}

void GraphicsShell::CalculateBvh ( ) { 
    auto scene_bvh = Scene::ConstructSceneBvh ( );
    AddBvhTreeToDebugGeometry ( scene_bvh, 1, 1 );
}

void GraphicsShell::SolveLighting ( ) {
    
}

void GraphicsShell::SolveLighting ( const LightSource & light_source ) {
    auto ray = light_source.GetRandomRay ( );

    AddRayToDebugGeometry ( ray );

}



void GraphicsShell::Render ( ) {
    DxShell::ClearFrameBuffer ( { 0.2f, 0.2f, 0.2f, 1.0f } );

    Scene::RenderForCamera (&mainCamera);

   int color = 255;

    DxShell::BindShader (lineShader);
    DxShell::BindCBV (0, mainCamera.GetViewMatrixBuffer());
    DxShell::BindVertexBuffer (debugGeometryVertexBuffer);
    DxShell::Draw (numDebugLines * 2, 1, 0);
    
    Gui::Render ( );


    DxShell::SetSwapChainToPresentState ( );
    DxShell::Execute ( );
    DxShell::ShowCurrentFrame ( );
}



void GraphicsShell::InitCamera ( ) {
    mainCamera = Camera ( { 0.0f, 0.0f, -10.0f },
                          { 0.0f, 0.0f, 1.0f },
                          { 0.0f, 1.0f, 0.0f }, 
                          3.14f / 3, aspect, 0.1f, 10000.0f );
    mainCamera.RotateXYAbsolute ( 0.0f, 0.0f );    
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
    /*texture_buffer = GraphicsShell::LoadDds ( "Bitmaps/2.dds", &width, &height );
    auto texture2 = DxShell::CreateTexture ( width, height, 1, DXGI_FORMAT_B8G8R8A8_UNORM );
    DxShell::CopyTexture ( texture2, texture_buffer );
    DxShell::CreateDescriptorForTexture ( texture2, 1 );
    DxShell::SetTextureAsPixelShaderInput ( texture2 );*/
}

void GraphicsShell::InitGui ( ) {
    /*auto panel_left =  Gui::CreatePanel( );
    panel_left->SetPosition ( { 10, 70 } );
    panel_left->SetSize ( { 300, 1000 } );
    panel_left->SetBgColor ( { 0.05f, 0.05f, 0.05f, 1.0f } );

    auto panel_right =  Gui::CreatePanel ( );
    panel_right->SetPosition ( { 10, 10 } );
    panel_right->SetSize ( { 1900, 50 } );
    panel_right->SetBgColor ( { 0.05f, 0.05f, 0.05f, 1.0f } );*/


    auto label = Gui::CreateLabel ( );
    label->SetColor ( { 1.0f,1.0f,0.3f,1.0f } );
    label->SetPosition ( { 100, 100 } );
    label->SetString ( "DirectX" );

    
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


