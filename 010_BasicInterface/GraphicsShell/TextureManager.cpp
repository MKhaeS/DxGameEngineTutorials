#include "TextureManager.h"

#include <fstream>

#include "dds.h"

void TextureManager::Initialize ( int num_textures ) {
}

DxTexture * TextureManager::CreateTextureFromDds ( std::string dds_filename ) {
    int width;
    int height;

    auto tx_buffer = CreateBufferFromDds ( dds_filename, &width, &height );
    if ( !tx_buffer ) {
        return nullptr;
    }
    auto texture   = DxShell::CreateTexture ( width, height, 1, DXGI_FORMAT_B8G8R8A8_UNORM );
    DxShell::CopyTexture ( texture, tx_buffer );

    DxShell::CreateDescriptorForTexture ( texture, numTextures );
    DxShell::SetTextureAsPixelShaderInput ( texture );

    numTextures++;
    return texture;
}

DxBuffer * TextureManager::CreateBufferFromDds ( std::string dds_filename, 
                                                 int * width, int * height ) {
    char                dwMagic[4];
    DDS_HEADER          header;

    std::ifstream dds_file ( dds_filename, std::ios::binary );
    dds_file.read ( reinterpret_cast<char*>(dwMagic), sizeof ( DWORD ) );
    dds_file.read ( reinterpret_cast<char*>(&header), sizeof ( DDS_HEADER ) );

    auto buffer_size = header.ddspf.dwSize * header.dwWidth * header.dwHeight / 8;
    auto tx_buffer   = DxShell::CreateBuffer ( buffer_size, DxBuffer::Type::CPU_BUFFER );
    auto buffer_ptr  = reinterpret_cast<char*>(tx_buffer->GetDataPointer ( ));


    dds_file.read ( buffer_ptr, buffer_size );
    if ( dds_file.fail ( ) ) {
        return nullptr;
    }
    *width  = header.dwWidth;
    *height = header.dwHeight;

    return tx_buffer;
}


