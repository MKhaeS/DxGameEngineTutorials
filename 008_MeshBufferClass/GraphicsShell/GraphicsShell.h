#pragma once

#include <string>
#include <fstream>

#include "..\DxShell\DxShell.h"
#include "MeshBuffer.h"
#include "Mesh.h"

#include "Camera.h"


class GraphicsShell {
public:
    static void Initialize ( HWND window_handle );    
    static void DeInitialize ( );


    static void KeyDown ( char key );
    static void KeyUp ( char key );

    static void Update ( );
    static void Render ( );

    static DxBuffer* LoadDds ( std::string filename, int* width, int* height );
    static Mesh*     LoadObj ( std::string filename );

private:
    static inline DxBuffer* uPlaneVertexBuffer = nullptr;
    static inline DxBuffer* uPlaneIndexBuffer = nullptr;
    static inline DxBuffer* positionBuffer = nullptr;


    static inline DxBuffer* meshVertexBuffer = nullptr;
    static inline DxBuffer* meshIndexBuffer = nullptr;
    static inline int       mesh_size = 0;

    static inline DxBuffer* bmp_buffer = nullptr;
    static inline DxShader* meshShader = nullptr;
    static inline DxShader* texturePlaneShader = nullptr;

    static inline int                 currentVBOffset  = 0;
    static inline int                 currentIBOffset  = 0;
    static inline Geometry::Vertex3d* currentVBPointer = nullptr;
    static inline int*                currentIBPointer = nullptr;


    static inline DxTexture* texture = nullptr;

    static inline Camera mainCamera;
    static inline DxBuffer* viewMatrixBuffer = nullptr;
    static inline DirectX::XMFLOAT4X4* viewMatrixPtr = nullptr;
    
    static inline HWND windowHandle;
    static inline int screen[2] = {};
    static inline float aspect = 1.0f;

    static inline POINT mousePosition;
    static inline bool wDown = false;
    static inline bool sDown = false;
    static inline bool aDown = false;
    static inline bool dDown = false;

    static void InitUnitPlane ( );
    static void InitCamera ( );
    static void InitUnitPlaneShader ( );
    static void InitMeshShader ( );
    static void InitTextures ( );
    static void InitMesh ( );







    GraphicsShell ( )                         = delete;
};