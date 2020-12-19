#pragma once

#include <cstdlib>

#include <DirectXMath.h>

#include "../Ray.h"



class LightSource {
public:
    DirectX::XMFLOAT3 GetPosition() const {
        return position;
    }

    Geometry::Ray GetRandomRay ( ) const {
        float u = static_cast <float> (rand ( )) / RAND_MAX;
        float v = static_cast <float> (rand ( )) / RAND_MAX;

        float theta = 2 * DirectX::XM_PI * u;
        float phi   = acos ( 2 * v - 1 );
        float x = sin ( phi ) * cos ( theta );
        float z = sin ( phi ) * sin ( theta );
        float y = cos ( phi );
        return { position, {x, y, z} };
    }

private:
    friend class Scene;
    DirectX::XMFLOAT3 position;
    float intencity;

    
};