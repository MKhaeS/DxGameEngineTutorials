#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "DxShell/DxShell.h"


LRESULT CALLBACK WndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void DxStuff ( );

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdShow, int nCmdShow ) {
    WNDCLASS basicWnd      = {};
    basicWnd.lpszClassName = "BasicWnd";
    basicWnd.hInstance     = hInstance;
    basicWnd.lpfnWndProc   = WndProc;
    basicWnd.style         = CS_VREDRAW | CS_HREDRAW;
    basicWnd.hCursor       = (HCURSOR)LoadCursor ( 0, IDC_ARROW );

    if ( !RegisterClass ( &basicWnd ) ) {
        return 0;
    }

    int width  = 1280;
    int height = 720;

    auto wndHandle = CreateWindow ( "BasicWnd", "Game", WS_OVERLAPPEDWINDOW,
                                    100, 100, width, height, 0, 0, hInstance, 0 );

    ShowWindow ( wndHandle, SW_SHOW );

    DxShell::Initialize ( wndHandle );
    auto vertex_buffer = DxShell::CreateBuffer ( 3 * sizeof ( Vertex2d ), DxBuffer::Type::CPU_BUFFER );
    void* buffer_pointer = vertex_buffer->GetDataPointer ( );

    auto vertices = reinterpret_cast<Vertex2d*>(buffer_pointer);

    vertices[0] = { {-1.0f, -0.000f } };
    vertices[2] = { { 0.0f, -0.000f } };
    vertices[1] = { { 0.0f,  1.0f } };

    vertex_buffer->SetAsVertexBuffer ( sizeof ( Vertex2d ) );

    auto shader = DxShell::CreateShader ( );
    shader->AssignInputLayot ( Vertex2d::INPUT_LAYOUT );
    shader->AssignVertexHlsl ( L"GraphicsShell/Shaders/Vertex/vs.hlsl" );
    shader->AssignPixelHlsl ( L"GraphicsShell/Shaders/Pixel/ps.hlsl" );
    DxShell::CommitShader ( shader );

    MSG msg = {};
    while ( msg.message != WM_QUIT ) {
        if ( PeekMessage ( &msg, 0, 0, 0, PM_REMOVE ) ) {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
        } else {
            DxShell::ClearFrameBuffer ( { 0.9f, 0.8f, 0.1f, 1.0f } );
            DxShell::BindShader ( shader );
            DxShell::BindBuffer ( vertex_buffer );
            DxShell::DrawInstanced ( 3 );
            //vertex_buffer.UnmapFromCpuMemory ( );

            DxShell::SetSwapChainToPresentState ( );
            DxShell::Execute ( );
            DxShell::ShowCurrentFrame ( );
            
        }
    }
    DxShell::Execute ( );
    return 0;
}

LRESULT CALLBACK WndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
    switch ( msg ) {
    case WM_DESTROY:
        PostQuitMessage ( 0 );
        break;
    }
    return DefWindowProc ( hWnd, msg, wParam, lParam );
}

