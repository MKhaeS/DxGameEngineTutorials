#include "GraphicsShell.h"


void GraphicsShell::Update ( ) {

    POINT new_mouse_position = {};
    GetCursorPos ( &new_mouse_position );
    ScreenToClient ( windowHandle, &new_mouse_position );

    POINT d_mouse_position = { mousePosition.x - new_mouse_position.x, 
                               mousePosition.y - new_mouse_position.y };



    float d_angleX = 0.0025f * (float ( d_mouse_position.x ));
    float d_angleY = 0.0025f * (float ( d_mouse_position.y ));

    mainCamera.RotateXYDelta ( d_angleY, -d_angleX );

    if ( wDown ) {
        mainCamera.MoveForward ( 0.1f );
    }
    if ( sDown ) {
        mainCamera.MoveForward ( -0.1f );
    }
    if ( aDown ) {
        mainCamera.MoveLeft ( 0.1f );
    }
    if ( dDown ) {
        mainCamera.MoveLeft ( -0.1f );
    }

    //(*viewMatrixPtr) = 
    mainCamera.UpdateViewMatrix ( );

    mousePosition = { screen[0] / 2, screen[1] / 2 };

    ClientToScreen( windowHandle, &mousePosition );

    SetCursorPos ( mousePosition.x, mousePosition.y );

    mousePosition = { screen[0] / 2, screen[1] / 2 };
}