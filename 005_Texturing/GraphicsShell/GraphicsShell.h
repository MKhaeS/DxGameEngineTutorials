#pragma once

#include <string>
#include <fstream>

#include "..\DxShell\DxShell.h"


class GraphicsShell {
public:
    static void Initialize ( HWND window_handle );    

    static void Render ( );

    static DxBuffer* LoadDds ( std::string filename, int* width, int* height );

private:
    static inline DxBuffer* vertexBuffer = nullptr;
    static inline DxBuffer* indexBuffer = nullptr;
    static inline DxBuffer* positionBuffer = nullptr;

    static inline DxBuffer* bmp_buffer = nullptr;
    static inline DxShader* shader = nullptr;

    static inline DxTexture* texture = nullptr;

    static void InitUnitPlane ( );


    static inline int screen[2] = {};
    static inline int size[2]   = {};

    static void InitUnitPlaneShader ( );




    GraphicsShell ( )                         = delete;
};