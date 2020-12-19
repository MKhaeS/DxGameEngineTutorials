#include "DxShell.h"
//#include "DxShader.h"


DxShader::DxShader ( ) {
    InitPipelineState ( );
    InitRootSignature ( );
}


void DxShader::InitPipelineState ( ) {
    rasterizerDescription.CullMode = D3D12_CULL_MODE_FRONT;
    rasterizerDescription.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    rasterizerDescription.FillMode  = D3D12_FILL_MODE_SOLID;
    rasterizerDescription.ForcedSampleCount = 0;
    rasterizerDescription.FrontCounterClockwise = TRUE;

    pipelineStateDescription.RasterizerState = rasterizerDescription;

    primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    pipelineStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateDescription.SampleMask = UINT_MAX;

    D3D12_RENDER_TARGET_BLEND_DESC rt_blend_desc {
        FALSE, FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL
    };

    D3D12_BLEND_DESC blend_desc = {};
    blend_desc.RenderTarget[0] = rt_blend_desc;

    pipelineStateDescription.BlendState     = blend_desc;

    pipelineStateDescription.NumRenderTargets = 1;
    pipelineStateDescription.RTVFormats[0]    = DXGI_FORMAT_R8G8B8A8_UNORM;
    pipelineStateDescription.SampleDesc.Count = 1;
}

void DxShader::InitRootSignature ( ) {
    rootSignatureDescription.Version        = D3D_ROOT_SIGNATURE_VERSION_1_1;
    rootSignatureDescription.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
}

void DxShader::AssignInputLayot ( const D3D12_INPUT_LAYOUT_DESC & layout_desc ) {
    this->layotDescription = layout_desc;
    pipelineStateDescription.InputLayout = this->layotDescription;
}

void DxShader::AssignVertexHlsl ( const std::wstring & vs_hlsl ) { 
    ID3DBlob* compile_error = nullptr;
    HRESULT res = D3DCompileFromFile ( vs_hlsl.data ( ), nullptr, nullptr, "main", "vs_5_1",
                                       D3DCOMPILE_DEBUG, 0, &vertexShader, &compile_error );
    if ( FAILED ( res ) ) {
        OutputDebugStringA ( (char*)compile_error->GetBufferPointer ( ) );
        return;
    }

    pipelineStateDescription.VS = { vertexShader->GetBufferPointer ( ),
                                    vertexShader->GetBufferSize ( ) };
}

void DxShader::AssignPixelHlsl ( const std::wstring & ps_hlsl ) {
    ID3DBlob* compile_error = nullptr;
    HRESULT res = D3DCompileFromFile ( ps_hlsl.data ( ), nullptr, nullptr, "main", "ps_5_1",
                                       D3DCOMPILE_DEBUG, 0, &pixelShader, &compile_error );
    if ( FAILED ( res ) ) {
        OutputDebugStringA ( (char*)compile_error->GetBufferPointer ( ) );
        return;
    }

    pipelineStateDescription.PS = { pixelShader->GetBufferPointer ( ),
                                    pixelShader->GetBufferSize ( ) };
}

DxShader * DxShell::CreateShader ( ) {
    DxShader shader;
    shaders.emplace_back ( shader );
    return &shaders.back();
}

void DxShell::CommitShader ( DxShader * shader ) {
    ID3DBlob* serialized_root_signature = nullptr;
    ID3DBlob* serialization_error       = nullptr;

    auto rs_description = &shader->rootSignatureDescription;
    opResult = D3D12SerializeVersionedRootSignature (rs_description, 
                                                     &serialized_root_signature, &serialization_error );
    if ( FAILED ( opResult ) ) {
        if ( serialization_error ) {
            OutputDebugStringA ( (char*)serialization_error->GetBufferPointer ( ) );
        }
        LogError ( opResult, "Failed to serialize the root signature. " );
        return;
    }

    ComPtr<ID3D12RootSignature> rs;
    opResult = device->CreateRootSignature ( 0, serialized_root_signature->GetBufferPointer ( ),
                                             serialized_root_signature->GetBufferSize ( ),
                                             IID_PPV_ARGS ( shader->rootSignature.GetAddressOf ( ) ) );

    if ( FAILED ( opResult ) ) {
        LogError ( opResult, "Failed to create the root signature. " );
        return;
    }

    auto pso_desc = &shader->pipelineStateDescription;
    pso_desc->pRootSignature = shader->rootSignature.Get ( );
    opResult = device->CreateGraphicsPipelineState ( pso_desc, IID_PPV_ARGS ( shader->pipelineState.GetAddressOf ( ) ) );
    if ( FAILED ( opResult ) ) {
        LogError ( opResult, "Failed to create the pipeline state object. " );
        return;
    }
}








