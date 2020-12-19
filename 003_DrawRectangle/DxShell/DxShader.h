#pragma once

#include "DxInclude.h"

#include <string>


class DxShader {
public:
    void AssignInputLayot ( const D3D12_INPUT_LAYOUT_DESC& layout_desc );
    void AssignVertexHlsl ( const std::wstring& vs_hlsl );
    void AssignPixelHlsl  ( const std::wstring& ps_hlsl );


    ~DxShader ( ) = default;
private:
    friend class DxShell;

    DxShader ( );

    ComPtr <ID3D12PipelineState>        pipelineState              = nullptr;
    ComPtr <ID3D12RootSignature>        rootSignature              = nullptr;

    D3D12_INPUT_LAYOUT_DESC             layotDescription            = {};
    D3D12_PRIMITIVE_TOPOLOGY            primitiveTopology           = {};
    D3D12_GRAPHICS_PIPELINE_STATE_DESC  pipelineStateDescription    = {};
    D3D12_RASTERIZER_DESC               rasterizerDescription       = {};

    D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription    = {};
             
    ID3DBlob*                           vertexShader                = nullptr;
    ID3DBlob*                           pixelShader                 = nullptr;

    void InitPipelineState ( );
    void InitRootSignature ( );
};