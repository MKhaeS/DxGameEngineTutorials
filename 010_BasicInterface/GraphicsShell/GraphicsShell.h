#pragma once

#include <string>
#include <fstream>

#include "..\DxShell\DxShell.h"

#include "Camera.h"

#include "Geometry/AABB.h"
#include "Geometry/Vertex.h"
#include "Geometry/Ray.h"

#include "Geometry/Scene/Scene.h"
#include "Gui/Gui.h"

constexpr auto DEBUG_GEOMETRY_BUFFER_SIZE = 100000;


class GraphicsShell {
public:
    static void  Initialize ( HWND window_handle );    
    static void  DeInitialize ( );


    static void  KeyDown ( char key );
    static void  KeyUp ( char key );

    static const Mesh*         AddMesh            ( std::string filename );
    static       MeshInstance* CreateMeshInstance ( const Mesh* mesh );
    static const LightSource*  CreateLightSource  ( DirectX::XMFLOAT3 position, float intencity );
    static void                UpdateAabbs  ( );
    static void                CalculateBvh ( );
    static void                SolveLighting ( );
    static void                SolveLighting ( const LightSource& light_source );

    static const void          SetMeshInstancePosition       ( MeshInstance* mesh_instance, float x, float y, float z );
    static void                MeshInstanceRotateY           ( MeshInstance* mesh_instance, float angle );
    static void                UpdateMeshInstanceModelMatrix ( MeshInstance* mesh_instance );

    static void  Update ( );
    static void  Render ( );


private:
    static void InitCamera ( );
    static void InitMeshShader ( );
    static void InitLineShader ( );
    static void InitTextures ( );
    static void InitGui ( );
    //
    static void InitDebugGeometryBuffers ( );
    static void AddAabbToDebugGeometry ( const Geometry::Aabb& aabb, DirectX::XMFLOAT3 color );
    static void AddBvhTreeToDebugGeometry ( const Geometry::Bvh* bvh,
                                            int min_depth, int max_depth,
                                            DirectX::XMFLOAT3 color = { 1.0f, 0.0f, 0.0f } );
    static void AddRayToDebugGeometry ( const Geometry::Ray& ray );
    static void AddLightSourceToDebugGeometry ( const LightSource& light_source );
    static void AddAxisToDebugGeometry ( );

    //static void InitMesh ( );



    // TEST UTILS


    static inline int                       mesh_size = 0;
                                            
    static inline DxBuffer*                 debugGeometryVertexBuffer = nullptr;
                                            
    static inline DxBuffer*                 bmp_buffer = nullptr;
    static inline DxShader*                 meshShader = nullptr;
    
    static inline DxShader*                 lineShader = nullptr;
                                            
    static inline Geometry::Vertex3dPosCol* debugGeometryVBPtr = nullptr;
    static inline int                       numDebugLines = 0;


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
    static inline bool eDown = false;
    static inline bool qDown = false;

    







    GraphicsShell ( )                         = delete;
};