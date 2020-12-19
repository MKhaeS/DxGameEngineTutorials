#pragma once

#define RETURN_IF_FAILED(function) {HRESULT res = function; if(FAILED(res)) return res;}

#define MAX_TEXTURES 1000

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgidebug.h>

#include <string>

#include <wrl.h>


using namespace Microsoft::WRL;

class DxShell {
public:
    static HRESULT Initialize                 ( HWND window_handle );
    static void    ClearFrameBuffer           ( DirectX::XMFLOAT4 bg_color );
    static void    SetSwapChainToPresentState ( );
    static void    Execute                    ( );
    static void    ShowCurrentFrame           ( );

private:
    DxShell ( )                          = delete;
    ~DxShell ( )                         = delete;
    DxShell ( const DxShell& )           = delete;
    DxShell ( DxShell&& )                = delete;
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
                                                                         
    static inline ComPtr<IDXGIFactory6>              factory             = nullptr;
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




    static inline HRESULT opResult = S_OK;
    static inline std::string lastError = "No error";

};
