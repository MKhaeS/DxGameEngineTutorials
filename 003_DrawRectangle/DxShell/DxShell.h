#pragma once

#include "DxInclude.h"

#include "DxBuffer.h"
#include "DxVertex.h"
#include "DxShader.h"

#include <string>
#include <vector>

class DxShell {
public:
    static HRESULT   Initialize                 ( HWND window_handle );
    static void      ClearFrameBuffer           ( DirectX::XMFLOAT4 bg_color );
    static void      SetSwapChainToPresentState ( );
    static void      Execute                    ( );
    static void      ShowCurrentFrame           ( );

    static DxBuffer*  CreateBuffer              ( UINT size, DxBuffer::Type buffer_type );

    static DxShader*  CreateShader              ( );
    static void       CommitShader              ( DxShader* shader );

    static void       BindShader ( DxShader* shader );
    static void       BindBuffer ( DxBuffer* buffer );

    static void       DrawInstanced (int num_vertices );

    

private:
    DxShell  ( )                         = delete;
    ~DxShell ( )                         = delete;
    DxShell  ( const DxShell& )          = delete;
    DxShell  ( DxShell&& )               = delete;
    DxShell& operator=( const DxShell& ) = delete;
    DxShell& operator=( DxShell&& )      = delete;

    static HRESULT CreateFactory ( );
    static HRESULT CreateDevice ( );
    static HRESULT CreateCommandStructures ( );
    static HRESULT CreateFence ( );
    static HRESULT CreateSwapChain ( HWND window_handle );
    static HRESULT CreateRenderTargets ( );
    static HRESULT CreateDepthStencilBuffer ( );
    static HRESULT CreateDepthStencilView ( );    
    static void    CreateViewPort ( );


    static void    LogError (HRESULT result, std::string error );


    static inline UINT rtvSize = 0;
    static inline UINT dsvSize = 0;
    static inline UINT srvSize = 0;

    static inline ComPtr<ID3D12Debug>                debug               = nullptr;
    static inline ComPtr<IDXGIDebug1>                debugInterface      = nullptr;
                                                                         
    static inline ComPtr<IDXGIFactory2>              factory             = nullptr;
    static inline ComPtr<IDXGIAdapter4>              adapter             = nullptr;
    static inline ComPtr<ID3D12Device>               device              = nullptr;
                                                                         
    static inline ComPtr<ID3D12CommandQueue>         cmdQueue            = nullptr;
    static inline ComPtr<ID3D12CommandAllocator>     cmdAllocator        = nullptr;
    static inline ComPtr<ID3D12GraphicsCommandList>  cmdList             = nullptr;
    static inline ComPtr<ID3D12Fence>                fence               = nullptr;
    static inline HANDLE                             fenceEvent          = {};
    static inline long long                          fenceValue          = 1;

    static inline UINT                               screenWidth         = 0;
    static inline UINT                               screenHeight        = 0;
    static inline ComPtr<IDXGISwapChain4>            swapChain           = nullptr;
    static inline ComPtr<ID3D12Resource>             swapChainBuffers[2] = {};
    static inline ComPtr<ID3D12DescriptorHeap>       rtvHeap             = nullptr;
    static inline D3D12_CPU_DESCRIPTOR_HANDLE        rtvCpuHandles[2]    = {};
    static inline DXGI_FORMAT                        depthStencilFormat  = DXGI_FORMAT_D32_FLOAT;
    static inline ComPtr<ID3D12Resource>             depthStencilBuffer  = nullptr;
    static inline ComPtr<ID3D12DescriptorHeap>       dsvHeap             = nullptr;
    static inline D3D12_CPU_DESCRIPTOR_HANDLE        dsvCpuHandle           = {};
    static inline D3D12_VIEWPORT                     viewport            = {};
    static inline D3D12_RECT                         scissorsRect        = {};

    static inline std::vector<DxBuffer>              dataBuffers         = {};
    static inline std::vector<DxShader>              shaders             = {};

    static inline HRESULT opResult = S_OK;
    static inline std::string lastError = "No error";
};
