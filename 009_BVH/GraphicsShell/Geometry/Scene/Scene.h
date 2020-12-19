#pragma once

#include <DirectXMath.h>
#include <string>

#include "../../../DxShell/DxShell.h"
#include "../Vertex.h"
#include "../AABB.h"
#include "../../Camera.h"

#include "Mesh.h"
#include "Bvh.h"



constexpr auto DEFAULT_NUM_VERTICES  = 1000000;
constexpr auto DEFAULT_NUM_INDICES   = 1000000;
constexpr auto DEFAULT_NUM_MESHES    = 100;
constexpr auto DEFAULT_NUM_INSTANCES = 100;


class Scene {
public:
    static void  Initialize         ( int num_vertices  = DEFAULT_NUM_VERTICES,
                                      int num_indices   = DEFAULT_NUM_INDICES,
                                      int num_meshes    = DEFAULT_NUM_MESHES,
                                      int num_instances = DEFAULT_NUM_INSTANCES*DEFAULT_NUM_MESHES );
    static void  DeInitialize       ();

    static Mesh* CreateMeshFromObj  ( std::string obj_filename );

    static MeshInstance*             CreateMeshInstance ( Mesh* mesh );
                                     
    static const DxBuffer*           GetModelMatrixBuffer ();
                                     
    static const DxBuffer*           GetVertexBuffer ();
                                     
    static const DxBuffer*           GetIndexBuffer ();
                                     
    static       void                RenderForCamera ( const Camera* camera );

    static const Geometry::Bvh*      ConstructBvhForMesh ( Mesh* mesh );

    static const Geometry::Bvh*      ConstructSceneBvh ();

    static const Geometry::Aabb&     GetMeshInstanceAabb (const MeshInstance* mesh_instance);

    static const void                SetMeshInstancePosition (MeshInstance* mesh_instance,
                                                              float x, float y, float z);




private:
    Scene ()                        = delete;
    ~Scene ()                       = delete;
    Scene (const Scene&)            = delete;
    Scene (Scene&&)                 = delete;
    Scene& operator= (const Scene&) = delete;
    Scene& operator= (Scene&&)      = delete;

    static inline Mesh* InitMesh ( int num_instances = 0 );
    static inline void  CalculateFaceAabb  ( int   face_idx );
    static inline void  CalculateMeshFacesAabbs ( Mesh* mesh );

    static inline void  CalculateMeshAabb (Mesh* mesh);

    static inline void  ConstructBvhForAabbArray (const std::vector<Geometry::AxisSortedAabb*>& aabbs,
                                                  Geometry::Bvh * bvh, Geometry::Bvh* (*init_function)() );

    static inline void  SortAabbsByEachAxis      ( std::vector<Geometry::AxisSortedAabb*>* sort_min_aabbs,
                                                  std::vector<Geometry::AxisSortedAabb*>* sort_max_aabbs );

    static inline void  ShrinkBvhNodeVolume      ( int axis, int aabb_idx, int n_aabbs,
                                                  std::vector<Geometry::AxisSortedAabb*>* sort_min_aabbs,
                                                  std::vector<Geometry::AxisSortedAabb*>* sort_max_aabbs,
                                                  int* min_idx, int* max_idx,
                                                  Geometry::Aabb* aabb_right, Geometry::AxisSortedAabb* right);

    static inline float CalculateSah            ( const Geometry::Aabb& left_aabb, 
                                                  const Geometry::Aabb& right_aabb,
                                                  int cur_aabb_idx, int num_aabbs );

    static inline Geometry::Bvh* InitFaceBvh ();

    static inline Geometry::Bvh* InitMeshBvh ();

    static inline void  InitializePbrShader ();


    static inline DxShader* pbrShader         = nullptr;


    static inline DxBuffer* vertexBuffer      = nullptr;
    static inline DxBuffer* indexBuffer       = nullptr;
    static inline DxBuffer* modelMatrixBuffer = nullptr;


    static inline Geometry::Vertex3d*  vbPointer                  = nullptr;
    static inline int*                 ibPointer                  = nullptr;
    static inline DirectX::XMFLOAT4X4* modelMatrixBufferPointer   = nullptr;
    static inline int                  curVbOffset                = 0;
    static inline int                  curIbOffset                = 0;
    static inline int                  curModelMatrixBufferOffset = 0;

    static inline Mesh*         meshes                 = nullptr;
    static inline MeshInstance* meshInstances          = nullptr;
    static inline int           numMeshes              = 0;
    static inline int           curMeshInstancesOffset = 0;

    static inline Geometry::LinkedAabb* meshInstanceAabbs     = nullptr;
    static inline int                   numMeshInstanceAabbs  = 0;

    static inline Geometry::LinkedAabb* faceAabbs         = nullptr;
    static inline int                   numFaceAabbs      = 0;
    static inline int                   curFaceAabbOffset = 0;

    static inline Geometry::Bvh*        faceBvhs          = nullptr;
    static inline int                   numFaceBvh        = 0;

    static inline Geometry::Bvh*        meshBvhs          = nullptr;
    static inline int                   numMeshBvhs       = 0;


};