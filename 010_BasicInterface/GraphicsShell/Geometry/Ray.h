#pragma once


#include <DirectXMath.h>


namespace Geometry {
    struct Ray {
        DirectX::XMFLOAT3 origin;
        DirectX::XMFLOAT3 direction;
    };
};