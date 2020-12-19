#pragma once

#include "DxInclude.h"

class DxTexture {
public:
    const int         width       = 0;
    const int         height      = 0;
    const int         mipLevels   = 0;
    const DXGI_FORMAT pixelFormat = {};

    ~DxTexture ( ) = default;
private:
    friend class DxShell;

    DxTexture ( ) = default;
    DxTexture ( int width, int height, int mip_levels, DXGI_FORMAT pixel_format );

    ComPtr<ID3D12Resource>          resource            = nullptr;
    D3D12_RESOURCE_DESC             description         = {};
    D3D12_HEAP_PROPERTIES           heapProperties      = {};

    //D3D12_GPU_VIRTUAL_ADDRESS       gpuAddress          = {};
    D3D12_RESOURCE_STATES           currentState        = {};

    D3D12_GPU_DESCRIPTOR_HANDLE     srvGpuHandle        = {};
};