#pragma once

#include <string>

#include "../DxShell/DxShell.h"

constexpr auto DEFAULT_NUM_TEXTURES = 100;

class TextureManager {
public:
    static void Initialize ( int num_textures = DEFAULT_NUM_TEXTURES );
    static DxTexture* CreateTextureFromDds ( std::string dds_filename );
private:
    static DxBuffer* CreateBufferFromDds ( std::string dds_filename,
                                           int* width, int* height );

    static inline int numTextures = 0;
};