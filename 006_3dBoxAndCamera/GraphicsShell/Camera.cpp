#include "Camera.h"

Camera::Camera ( DirectX::XMFLOAT3A position,
                 DirectX::XMFLOAT3A look_at,
                 DirectX::XMFLOAT3A up,
                 float              fovy,
                 float              aspect,
                 float              near_z,
                 float              far_z )
    :
    position ( position ),
    lookAt ( look_at ),
    up ( up ),
    fovy ( fovy ),
    aspect ( aspect ),
    nearZ ( near_z ),
    farZ ( far_z ) {

    auto eye    = DirectX::XMLoadFloat3A ( &this->position );
    auto focus  = DirectX::XMLoadFloat3A ( &this->lookAt );
    auto up_dir = DirectX::XMLoadFloat3A ( &this->up );

    auto direction = DirectX::XMLoadFloat3A ( &lookAtVector );
    direction = DirectX::XMVectorSubtract ( focus, eye );
    direction = DirectX::XMVector3Normalize ( direction );

    DirectX::XMStoreFloat3A ( &lookAtVector, direction );
}

void Camera::RotateXYAbsolute ( float angle_x, float angle_y ) {
    angleX = angle_x;
    angleY = angle_y;

    auto eye    = DirectX::XMLoadFloat3A ( &this->position );
    auto focus  = DirectX::XMLoadFloat3A ( &this->lookAt );
    auto up_dir = DirectX::XMLoadFloat3A ( &this->up );


    lookAtVector = { sinf ( angleY )*cosf ( angleX ), sinf ( angleX ), cosf ( angleY ) * cosf ( angleX ) };
    focus = DirectX::XMLoadFloat3A ( &lookAtVector );
    focus = DirectX::XMVectorAdd ( focus, eye );

    DirectX::XMStoreFloat3A ( &lookAt, focus );

    this->left.x = -lookAtVector.z;
    this->left.z = lookAtVector.x;

    auto left_ = DirectX::XMLoadFloat3A ( &this->left );
    left_ = DirectX::XMVector3Normalize ( left_ );
    DirectX::XMStoreFloat3A ( &this->left, left_ );
}

void Camera::RotateXYDelta ( float d_angle_x, float d_angle_y ) {
    angleX += d_angle_x;
    angleY += d_angle_y;

    auto eye    = DirectX::XMLoadFloat3A ( &this->position );
    auto focus  = DirectX::XMLoadFloat3A ( &this->lookAt );
    auto up_dir = DirectX::XMLoadFloat3A ( &this->up );


    lookAtVector = { sinf ( angleY )*cosf ( angleX ), sinf ( angleX ), cosf ( angleY ) * cosf ( angleX ) };
    focus = DirectX::XMLoadFloat3A ( &lookAtVector );
    focus = DirectX::XMVectorAdd ( focus, eye );

    DirectX::XMStoreFloat3A ( &lookAt, focus );

    this->left.x = -lookAtVector.z;
    this->left.z = lookAtVector.x;

    auto left_ = DirectX::XMLoadFloat3A ( &this->left );
    left_ = DirectX::XMVector3Normalize ( left_ );
    DirectX::XMStoreFloat3A ( &this->left, left_ );
}

void Camera::MoveForward ( float step ) {
    auto eye    = DirectX::XMLoadFloat3A ( &this->position );
    auto focus  = DirectX::XMLoadFloat3A ( &this->lookAt );
    auto up_dir = DirectX::XMLoadFloat3A ( &this->up );

    DirectX::XMFLOAT3A step_ = { step, step, step };
    auto step_vector = DirectX::XMLoadFloat3A ( &step_ );
    auto direction = DirectX::XMLoadFloat3A ( &lookAtVector );
    direction = DirectX::XMVectorMultiply ( direction, step_vector );

    focus = DirectX::XMVectorAdd ( focus, direction );
    eye   = DirectX::XMVectorAdd ( eye, direction );

    DirectX::XMStoreFloat3A ( &lookAt, focus );
    DirectX::XMStoreFloat3A ( &position, eye );
}

void Camera::MoveLeft ( float step ) { 
    auto eye    = DirectX::XMLoadFloat3A ( &this->position );
    auto focus  = DirectX::XMLoadFloat3A ( &this->lookAt );
    auto up_dir = DirectX::XMLoadFloat3A ( &this->up );

    DirectX::XMFLOAT3A step_ = { step, step, step };
    auto step_vector = DirectX::XMLoadFloat3A ( &step_ );
    auto direction = DirectX::XMLoadFloat3A ( &left );
    direction = DirectX::XMVectorMultiply ( direction, step_vector );

    focus = DirectX::XMVectorAdd ( focus, direction );
    eye   = DirectX::XMVectorAdd ( eye, direction );

    DirectX::XMStoreFloat3A ( &lookAt, focus );
    DirectX::XMStoreFloat3A ( &position, eye );
}

DirectX::XMFLOAT4X4A Camera::GetViewMatrix ( ) {
    auto eye    = DirectX::XMLoadFloat3A ( &this->position );
    auto focus  = DirectX::XMLoadFloat3A ( &this->lookAt );
    auto up_dir = DirectX::XMLoadFloat3A ( &this->up );

    auto view_matrix = DirectX::XMMatrixLookAtLH ( eye, focus, up_dir );
    view_matrix *= DirectX::XMMatrixPerspectiveFovLH ( this->fovy, this->aspect, nearZ, farZ );

    view_matrix = DirectX::XMMatrixTranspose ( view_matrix );
    DirectX::XMStoreFloat4x4A ( &this->viewMatrix, view_matrix );
    return viewMatrix;
}
