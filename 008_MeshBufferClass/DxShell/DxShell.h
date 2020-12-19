#pragma once

#include "DxInclude.h"

#include "DxBuffer.h"
#include "DxTexture.h"
#include "DxShader.h"


#include <string>
#include <vector>

#define A_NUMBER 100

class DxShell {
public:
    static HRESULT   Initialize                 ( HWND window_handle );
    static void      ClearFrameBuffer           ( DirectX::XMFLOAT4 bg_color );
    static void      SetSwapChainToPresentState ( );
    static void      Execute                    ( );
    static void      ShowCurrentFrame           ( );

    static DxBuffer*  CreateBuffer              ( UINT size, DxBuffer::Type buffer_type );
    static void       CopyBuffer                ( DxBuffer* dst_buffer, DxBuffer* src_buffer,
                                                  UINT size, UINT offset_dst, UINT offset_src );

    static DxTexture* CreateTexture ( int width, int height, int mip_levels, DXGI_FORMAT pixel_format );
    static void       CopyTexture ( DxTexture* dst_texture, DxBuffer* src_buffer );
    static bool       CreateDescriptorForTexture ( DxTexture* texture, int descriptor_idx );
    static void       SetTextureAsPixelShaderInput ( DxTexture* texture );


    static DxShader*  CreateShader              ( );
    static void       CommitShader              ( DxShader* shader );

    static void       BindShader        ( DxShader* shader );
    static void       BindVertexBuffer  ( DxBuffer* buffer );
    static void       BindIndexBuffer   ( DxBuffer* buffer );
    static void       BindConstants     ( UINT parameter_idx, UINT size, void* data, UINT offset );
    static void       BindCBV           ( UINT parameter_idx, DxBuffer* buffer );
    static void       BindTextureArray  ( UINT parameter_idx, DxTexture* texture );
    static void       Draw              ( int num_indices, int num_instances, int vb_offset, int ib_offset );

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

    static HRESULT CreateSrvHeap ( );

    static void    ChangeBufferState ( DxBuffer* buffer, D3D12_RESOURCE_STATES new_state );
    static void    ChangeTextureState ( DxTexture* texture, D3D12_RESOURCE_STATES new_state );


    static void    LogError (HRESULT result, std::string error );


    static inline UINT rtvSize = 0;
    static inline UINT dsvSize = 0;
    static inline UINT srvSize = 0;

    static const  UINT NUM_SRV_DESCRIPTORS = 10;

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

    static inline ComPtr<ID3D12DescriptorHeap>       srvHeap             = nullptr;
    static inline D3D12_GPU_DESCRIPTOR_HANDLE        srvGpuHandle        = {};
    static inline D3D12_CPU_DESCRIPTOR_HANDLE        srvCpuHandle        = {};

    static inline DxBuffer                           dataBuffers[A_NUMBER]     = {};
    static inline DxShader                           shaders[A_NUMBER]         = {};
    static inline DxTexture                          textures[A_NUMBER]        = {};
    static inline UINT                               num_buffers         = 0;
    static inline UINT                               numShaders          = 0;
    static inline int                                num_textures        = 0;


    static inline HRESULT     opResult = S_OK;
    static inline std::string lastError = "No error";
};
