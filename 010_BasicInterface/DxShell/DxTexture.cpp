#include "DxTexture.h"
#include "DxShell.h"

DxTexture::DxTexture ( int width, int height, int mip_levels, DXGI_FORMAT pixel_format ) {
    description.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    description.Width            = width;
    description.Height           = height;
    description.DepthOrArraySize = 1;
    description.MipLevels        = mip_levels;
    description.Format           = pixel_format;
    description.SampleDesc.Count = 1;

    /*srvDescription.ViewDimension             = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDescription.Shader4ComponentMapping   = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDescription.Format                    = pixelFormat;
    srvDescription.Texture2D.MipLevels       = mip_levels;
    srvDescription.Texture2D.MostDetailedMip = 0;*/

    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    currentState = D3D12_RESOURCE_STATE_COPY_DEST;
}

DxTexture * DxShell::CreateTexture ( int width, int height, int mip_levels, DXGI_FORMAT pixel_format ) {
    auto texture = &textures[numTextures];
    texture = new (texture) DxTexture ( width, height, mip_levels, pixel_format );
    numTextures++;

    opResult = device->CreateCommittedResource ( &texture->heapProperties,
                                                 D3D12_HEAP_FLAG_NONE,
                                                 &texture->description,
                                                 texture->currentState,
                                                 nullptr,
                                                 IID_PPV_ARGS ( texture->resource.GetAddressOf ( ) ) );
    if ( FAILED ( opResult ) ) {
        LogError ( opResult, "Failed to create the texture, HRESULT = " );
        return nullptr;
    }

    return texture;
}

void DxShell::CopyTexture ( DxTexture * dst_texture, DxBuffer * src_buffer ) { 
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT mip_footprint = {};
    UINT64 data_size = 0;
    device->GetCopyableFootprints ( &dst_texture->description,
                                    0, 1, 0, &mip_footprint, 0, 0, &data_size );

    D3D12_TEXTURE_COPY_LOCATION tx_src_location = {};
    tx_src_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    tx_src_location.pResource = src_buffer->resource.Get ( );
    tx_src_location.PlacedFootprint = mip_footprint;


    D3D12_TEXTURE_COPY_LOCATION tx_dst_location = {};
    tx_dst_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    tx_dst_location.pResource = dst_texture->resource.Get ( );
    tx_dst_location.SubresourceIndex = 0;

    cmdList->CopyTextureRegion ( &tx_dst_location, 0, 0, 0, &tx_src_location, 0 );
}

bool DxShell::CreateDescriptorForTexture ( DxTexture * texture, int descriptor_idx ) {
    if ( descriptor_idx >= NUM_SRV_DESCRIPTORS ) {
        LogError ( E_FAIL, "Failed to create the descriptor, HRESULT = " );
        return false;
    }
    
    device->CreateShaderResourceView ( texture->resource.Get ( ), nullptr,
                                       { srvCpuHandle.ptr + srvSize * descriptor_idx } );
    texture->srvGpuHandle = { srvGpuHandle.ptr + srvSize * descriptor_idx };

    return true;
}

void DxShell::ChangeTextureState ( DxTexture * texture, D3D12_RESOURCE_STATES new_state ) {
    if ( texture->currentState == new_state ) {
        return;
    }
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource   = texture->resource.Get ( );
    barrier.Transition.StateBefore = texture->currentState;
    barrier.Transition.StateAfter  = new_state;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

    // !!! - Not effective, batch the barriers -- 
    cmdList->ResourceBarrier ( 1, &barrier );
    texture->currentState = new_state;
}

void DxShell::SetTextureAsPixelShaderInput ( DxTexture * texture ) {
    ChangeTextureState ( texture, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE );
}




