#include "DxShell.h"


HRESULT DxShell::Initialize ( HWND window_handle ) {
    HRESULT res = S_OK;

    res = CreateFactory ( );
    if ( FAILED ( res ) ) {
        LogError ( res, "Failed to create the DX factory, HRESULT = " );
        return res;
    }

    res = CreateDevice ( );
    if ( FAILED ( res ) ) {
        LogError ( res, "Failed to create the DX device, HRESULT = " );
        return res;
    }

    /*D3D12_FEATURE_DATA_D3D12_OPTIONS7 featureData = {};
    device->CheckFeatureSupport (D3D12_FEATURE_D3D12_OPTIONS7, &featureData, sizeof (featureData));*/

    res = CreateCommandStructures ( );
    if ( FAILED ( res ) ) {
        LogError ( res, "Failed to create the command structures, HRESULT = " );
        return res;
    }

    res = CreateFence ( );
    if ( FAILED ( res ) ) {
        LogError ( res, "Failed to create the fence, HRESULT = " );
        return res;
    }

    res = CreateSwapChain ( window_handle );
    if ( FAILED ( res ) ) {
        LogError ( res, "Failed to create the swap chain, HRESULT = " );
        return res;
    }

    res = CreateRenderTargets ( );
    if ( FAILED ( res ) ) {
        LogError ( res, "ailed to create the render targets, HRESULT = " );
        return res;
    }

    res = CreateDepthStencilBuffer ( );
    if ( FAILED ( res ) ) {
        LogError ( res, "Failed to create the depth/stencil buffer, HRESULT = " );
        return res;
    }

    res = CreateDepthStencilView ( );
    if ( FAILED ( res ) ) {
        LogError ( res, "Failed to create the depth/stencil view, HRESULT = " );
        return res;
    }

    CreateViewPort ( );

    res = CreateSrvHeap ( );
    if ( FAILED ( res ) ) {
        LogError ( res, "Failed to create the shader resource view heap, HRESULT = " );
        return res;
    }


    OutputDebugString ( (lastError + "\n").data ( ) );
    return S_OK;
}

HRESULT DxShell::CreateFactory ( ) {
    int creation_flag  = 0;
    HRESULT res        = S_OK;

#ifdef DX_DEBUG
    res = D3D12GetDebugInterface ( IID_PPV_ARGS ( debug.GetAddressOf ( ) ) );
    debug->EnableDebugLayer ( );
    creation_flag = DXGI_CREATE_FACTORY_DEBUG;
    DXGIGetDebugInterface1 ( 0, IID_PPV_ARGS ( debugInterface.GetAddressOf ( ) ) );
#endif
    //CreateDXGIFactory ( IID_PPV_ARGS ( factory.GetAddressOf ( ) ) );
    RETURN_IF_FAILED ( CreateDXGIFactory2 ( creation_flag, IID_PPV_ARGS ( factory.GetAddressOf() ) ) );

    return res;
}

HRESULT DxShell::CreateDevice ( ) {
    ComPtr<IDXGIAdapter1> temp_adapter = nullptr;
    factory->EnumAdapters1 ( 0, &temp_adapter );
    adapter = static_cast<IDXGIAdapter4*>(temp_adapter.Get ( ));
    return D3D12CreateDevice ( adapter.Get ( ), D3D_FEATURE_LEVEL_12_1,
                               IID_PPV_ARGS ( &device ) );
}

HRESULT DxShell::CreateCommandStructures ( ) {
    HRESULT res = S_OK;

    D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = {};
    cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    RETURN_IF_FAILED ( device->CreateCommandQueue ( &cmd_queue_desc,
                                                    IID_PPV_ARGS ( cmdQueue.GetAddressOf ( ) ) ) );   
    RETURN_IF_FAILED ( device->CreateCommandAllocator ( D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                        IID_PPV_ARGS ( cmdAllocator.GetAddressOf ( ) ) ) );   
    RETURN_IF_FAILED ( device->CreateCommandList ( 0, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                   cmdAllocator.Get ( ), nullptr,
                                                   IID_PPV_ARGS ( cmdList.GetAddressOf ( ) ) ) );    
    RETURN_IF_FAILED( cmdList->Close ( ) );
    RETURN_IF_FAILED( cmdAllocator->Reset ( ) );
    RETURN_IF_FAILED( cmdList->Reset ( cmdAllocator.Get ( ), nullptr ) );
    
    return S_OK;
}

HRESULT DxShell::CreateFence ( ) {
    fenceEvent = CreateEvent ( nullptr, false, false, nullptr );
    fenceValue = 1;
    return device->CreateFence ( 0, D3D12_FENCE_FLAG_NONE,
                                 IID_PPV_ARGS ( fence.GetAddressOf ( ) ) );
}

HRESULT DxShell::CreateSwapChain ( HWND window_handle ) {
    HRESULT res = S_OK;

    RECT wnd_rect;
    if ( !GetClientRect ( window_handle, &wnd_rect ) )
        return E_FAIL;

    screenWidth  = wnd_rect.right - wnd_rect.left;
    screenHeight = wnd_rect.bottom - wnd_rect.top;

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    swap_chain_desc.BufferCount       = 2;
    swap_chain_desc.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.Format            = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.Width             = screenWidth;
    swap_chain_desc.Height            = screenHeight;
    swap_chain_desc.SampleDesc.Count  = 1;
    swap_chain_desc.SwapEffect        = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    ComPtr<IDXGISwapChain1> tempSwapChain = nullptr;

    res = factory->CreateSwapChainForHwnd ( cmdQueue.Get ( ), window_handle,
                                            &swap_chain_desc, nullptr, nullptr,
                                            &tempSwapChain );

    tempSwapChain.As ( &swapChain );
    return res;
}

HRESULT DxShell::CreateRenderTargets ( ) {
    HRESULT res = S_OK;

    rtvSize = device->GetDescriptorHandleIncrementSize ( D3D12_DESCRIPTOR_HEAP_TYPE_RTV );

    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.NumDescriptors = 2;

    RETURN_IF_FAILED( device->CreateDescriptorHeap ( &rtv_heap_desc,
                                         IID_PPV_ARGS ( rtvHeap.GetAddressOf ( ) ) ) );

    swapChain->GetBuffer ( 0, IID_PPV_ARGS ( swapChainBuffers[0].GetAddressOf ( ) ) );
    swapChain->GetBuffer ( 1, IID_PPV_ARGS ( swapChainBuffers[1].GetAddressOf ( ) ) );
    swapChainBuffers[0]->SetName ( L"Swap Chain Buffer 0" );
    swapChainBuffers[1]->SetName ( L"Swap Chain Buffer 1" );

    rtvCpuHandles[0] = rtvHeap->GetCPUDescriptorHandleForHeapStart ( );

    rtvCpuHandles[1] = rtvCpuHandles[0];
    rtvCpuHandles[1].ptr +=rtvSize;

    device->CreateRenderTargetView ( swapChainBuffers[0].Get ( ), nullptr,
                                     rtvCpuHandles[0] );
    device->CreateRenderTargetView ( swapChainBuffers[1].Get ( ), nullptr,
                                     rtvCpuHandles[1] );
    return S_OK;
}

HRESULT DxShell::CreateDepthStencilBuffer ( ) {
    HRESULT res = S_OK;

    dsvSize = device->GetDescriptorHandleIncrementSize ( D3D12_DESCRIPTOR_HEAP_TYPE_DSV );

    D3D12_RESOURCE_DESC ds_resource_desc = {};
    ds_resource_desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    ds_resource_desc.Width            = screenWidth;
    ds_resource_desc.Height           = screenHeight;
    ds_resource_desc.DepthOrArraySize = 1;
    ds_resource_desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    ds_resource_desc.Format           = depthStencilFormat;
    ds_resource_desc.MipLevels        = 1;
    ds_resource_desc.SampleDesc.Count = 1;

    D3D12_HEAP_PROPERTIES ds_heap_properties = {};
    ds_heap_properties.Type = D3D12_HEAP_TYPE_DEFAULT;

    D3D12_CLEAR_VALUE ds_clear_value = {};
    ds_clear_value.Format               = depthStencilFormat;
    ds_clear_value.DepthStencil.Depth   = 1.0f;
    ds_clear_value.DepthStencil.Stencil = 0;

    res = device->CreateCommittedResource ( &ds_heap_properties, D3D12_HEAP_FLAG_NONE,
                                            &ds_resource_desc,
                                            D3D12_RESOURCE_STATE_DEPTH_WRITE, &ds_clear_value,
                                            IID_PPV_ARGS ( depthStencilBuffer.GetAddressOf ( ) ) );
    depthStencilBuffer->SetName ( L"Depth/Stencil buffer" );
    return res;
}

HRESULT DxShell::CreateDepthStencilView ( ) {
    D3D12_DESCRIPTOR_HEAP_DESC ds_heap_desc = {};
    ds_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    ds_heap_desc.NumDescriptors = 1;

    RETURN_IF_FAILED( device->CreateDescriptorHeap ( &ds_heap_desc, 
                                                     IID_PPV_ARGS ( dsvHeap.GetAddressOf ( ) ) ) );

    dsvCpuHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart ( );
    device->CreateDepthStencilView ( depthStencilBuffer.Get ( ), nullptr, dsvCpuHandle );

    return opResult;
}

void    DxShell::CreateViewPort ( ) {
    viewport.Width    = (float)screenWidth;
    viewport.Height   = (float)screenHeight;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    scissorsRect.top    = 0;
    scissorsRect.left   = 0;
    scissorsRect.bottom = screenHeight;
    scissorsRect.right  = screenWidth;
}

HRESULT DxShell::CreateSrvHeap ( ) {
    srvSize = device->GetDescriptorHandleIncrementSize ( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
    D3D12_DESCRIPTOR_HEAP_DESC srv_heap_desc = {};
    srv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srv_heap_desc.NumDescriptors = NUM_SRV_DESCRIPTORS;
    srv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    RETURN_IF_FAILED ( device->CreateDescriptorHeap ( &srv_heap_desc,
                                                      IID_PPV_ARGS ( srvHeap.GetAddressOf ( ) ) ) );
    srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart ( );
    srvCpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart ( );
    return S_OK;
}



