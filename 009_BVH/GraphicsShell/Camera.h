#pragma once

#include <DirectXMath.h>

#include "../DxShell/DxShell.h"


class Camera {
public:
    Camera ( DirectX::XMFLOAT3A position,
             DirectX::XMFLOAT3A look_at,
             DirectX::XMFLOAT3A up,
             float              fovy,
             float              aspect,
             float              near_z,
             float              far_z );

    Camera ( ) = default;

    void RotateXYAbsolute ( float angle_x, float angle_y );
    void RotateXYDelta    ( float d_angle_x, float d_angle_y );
    void MoveForward      ( float step );
    void MoveLeft         ( float step );

    DirectX::XMFLOAT4X4 GetViewMatrix ( ) const;
    void                UpdateViewMatrix ();
    const DxBuffer*     GetViewMatrixBuffer () const;

private:
    DirectX::XMFLOAT3A position = {};
    DirectX::XMFLOAT3A lookAt   = {};
    DirectX::XMFLOAT3A up       = {};
    float              fovy     = DirectX::XM_PI / 3;
    float              aspect   = 1.0f;
    float              nearZ    = 0.1f;
    float              farZ     = 10000.0f;

    float              angleY   = 0.0f;
    float              angleX   = 0.0f;
    DirectX::XMFLOAT3A lookAtVector = {};
    DirectX::XMFLOAT3A left         = {};
    
    static inline DxBuffer*            viewMatrixBuffer = nullptr;
    static inline DirectX::XMFLOAT4X4* viewMatrixPtr    = nullptr;
};
