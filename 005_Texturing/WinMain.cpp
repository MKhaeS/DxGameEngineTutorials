#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "GraphicsShell/GraphicsShell.h"

LRESULT CALLBACK WndProc ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

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

    GraphicsShell::Initialize ( wndHandle );
   
    MSG msg = {};
    while ( msg.message != WM_QUIT ) {
        if ( PeekMessage ( &msg, 0, 0, 0, PM_REMOVE ) ) {
            TranslateMessage ( &msg );
            DispatchMessage ( &msg );
        } else {
            GraphicsShell::Render ( );
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

