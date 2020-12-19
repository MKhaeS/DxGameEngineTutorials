#pragma once

#include "../../DxShell/DxInclude.h"

namespace Geometry {
    struct Vertex2d {
        DirectX::XMFLOAT2 Position;
        DirectX::XMFLOAT2 TextCoord;

        static inline const int NUM_ELEMENTS = 2;
        static inline const D3D12_INPUT_ELEMENT_DESC INPUT_ELEMENTS[NUM_ELEMENTS] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        };
        static inline D3D12_INPUT_LAYOUT_DESC INPUT_LAYOUT = {
            &INPUT_ELEMENTS[0], NUM_ELEMENTS
        };
    };


    struct Vertex3d {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Normal;
        DirectX::XMFLOAT2 TextCoord;

        static inline const int NUM_ELEMENTS = 3;
        static inline const D3D12_INPUT_ELEMENT_DESC INPUT_ELEMENTS[NUM_ELEMENTS] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };
        static inline D3D12_INPUT_LAYOUT_DESC INPUT_LAYOUT = {
            &INPUT_ELEMENTS[0], NUM_ELEMENTS
        };
    };

    struct Vertex3dPosCol {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Color;

        static inline const int NUM_ELEMENTS = 2;
        static inline const D3D12_INPUT_ELEMENT_DESC INPUT_ELEMENTS[NUM_ELEMENTS] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };
        static inline D3D12_INPUT_LAYOUT_DESC INPUT_LAYOUT = {
            &INPUT_ELEMENTS[0], NUM_ELEMENTS
        };
    };
}

/*{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }*/