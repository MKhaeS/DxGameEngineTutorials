#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "GraphicsShell/GraphicsShell.h"



LRESULT CALLBACK WndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
void UserActions ( );

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

    int width  = 1920;
    int height = 1080;
    auto wndHandle = CreateWindow ( "BasicWnd", "Game", WS_POPUP/* WS_OVERLAPPEDWINDOW*/,
                                    0, 0, width, height, 0, 0, hInstance, 0 );

    ShowWindow ( wndHandle, SW_SHOW );

    GraphicsShell::Initialize ( wndHandle );

    UserActions ( );
   
    MSG msg = {};
    while ( msg.message != WM_QUIT ) {
        if ( PeekMessage ( &msg, 0, 0, 0, PM_REMOVE ) ) {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
        } else {
            GraphicsShell::Update ( );
            GraphicsShell::Render ( );
        }
    }
    DxShell::Execute ( );

    GraphicsShell::DeInitialize ( );
    return 0;
}

LRESULT CALLBACK WndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {
    switch ( msg ) {
    case WM_KEYDOWN:
        GraphicsShell::KeyDown ( wParam );
        break;
    case WM_KEYUP:
        GraphicsShell::KeyUp ( wParam );
        break;
    case WM_DESTROY:
        PostQuitMessage ( 0 );
        break;
    }
    return DefWindowProc ( hWnd, msg, wParam, lParam );
}


void UserActions ( ) {


    /*auto box_mesh = GraphicsShell::AddMesh ( "Mesh/box.vb" );
    for ( int i = 0; i < 10; i++ ) {
        auto box_instance = GraphicsShell::CreateMeshInstance ( box_mesh );

        auto position_x = (float)(rand ( ) % 1000);
        auto position_y = (float)(rand ( ) % 1000);
        auto position_z = (float)(rand ( ) % 1000);

        GraphicsShell::SetMeshInstancePosition ( box_instance, position_x, position_y, position_z );
        GraphicsShell::UpdateMeshInstanceModelMatrix ( box_instance );
    }*/

    

    auto surface_mesh = Scene::CreateSurfaceMesh(100);
    auto surface_instance = GraphicsShell::CreateMeshInstance(surface_mesh);
    GraphicsShell::SetMeshInstancePosition(surface_instance, 0.0f, 0.0f, 0.0f);
    GraphicsShell::UpdateMeshInstanceModelMatrix(surface_instance);

    /*auto box_instance = GraphicsShell::CreateMeshInstance ( box_mesh );

    GraphicsShell::SetMeshInstancePosition ( box_instance, 2000.0f, 0.0, 2000.0f );
    GraphicsShell::MeshInstanceRotateY ( box_instance, DirectX::XM_PIDIV4 );
    GraphicsShell::UpdateMeshInstanceModelMatrix ( box_instance );
    
    auto sphere_mesh = GraphicsShell::AddMesh ( "Mesh/Sphere.vb" );

    for ( int i = 0; i < 10; i++ ) {
        auto sphere_instance = GraphicsShell::CreateMeshInstance ( sphere_mesh );

        auto position_x = (float)(rand ( ) % 1000);
        auto position_y = (float)(rand ( ) % 1000);
        auto position_z = (float)(rand ( ) % 1000);

        GraphicsShell::SetMeshInstancePosition ( sphere_instance, position_x, position_y, position_z );
        GraphicsShell::UpdateMeshInstanceModelMatrix ( sphere_instance );
    }
    
    GraphicsShell::CalculateBvh ( );

    auto light_source = GraphicsShell::CreateLightSource ( { 500.0f, 200.0f, 500.0f }, 1.0f );

    GraphicsShell::SolveLighting ( *light_source );*/
}
