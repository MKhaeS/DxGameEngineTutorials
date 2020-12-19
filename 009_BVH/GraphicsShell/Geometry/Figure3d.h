#ifndef _FIGURE3D_H
#define _FIGURE3D_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <scene.h>
#include <Importer.hpp>
#include <postprocess.h>



#include "Polygon.h"

#pragma comment(lib, "assimp-vc140-mt.lib")


namespace Geometry
{
    class Figure3d
    {
    public:
        Figure3d();
        Figure3d(const std::vector<std::shared_ptr<Polygon>>& polygons);

        int NumOfPolygons() const;
        const Polygon& GetPolygon(int idx) const;

        static Figure3d CreateFromObj(const std::string& filepath, Shader* sh);

        ~Figure3d();
    private:
        std::vector<std::shared_ptr<Polygon>> m_Polygons;

        static std::vector<std::shared_ptr<Polygon>> GetAssimpNodePolygons(aiNode* node, const aiScene* scene, Shader* sh);

    };


    Figure3d::~Figure3d() {}
    Figure3d::Figure3d() {}

    Figure3d::Figure3d(const std::vector<std::shared_ptr<Polygon>>& polygons) {
        m_Polygons.clear();
        for (int i = 0; i < polygons.size(); ++i) {
            m_Polygons.push_back(polygons[i]);
        }
    }

    int Figure3d::NumOfPolygons() const {
        return m_Polygons.size();
    }

    const Polygon & Figure3d::GetPolygon(int idx) const {
        if (idx < m_Polygons.size())
            return  *m_Polygons[idx];
        else
            return Polygon();
    }



    Figure3d Figure3d::CreateFromObj(const std::string& filepath, Shader* sh) {
        Assimp::Importer obj_importer;
        const aiScene* scene = obj_importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP::" << obj_importer.GetErrorString() << std::endl;
            return Figure3d();
        }
        return Figure3d(GetAssimpNodePolygons(scene->mRootNode, scene, sh));
    }


    std::vector<std::shared_ptr<Polygon>> Figure3d::GetAssimpNodePolygons(aiNode * node, const aiScene * scene, Shader* sh) {
        std::vector<std::shared_ptr<Polygon>> polygons;
        std::vector<Vertex> verts;
        for (int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            for (int i = 0; i < mesh->mNumVertices; ++i) {
                float x, y, z;
                x = mesh->mVertices[i].x;
                y = mesh->mVertices[i].y;
                z = mesh->mVertices[i].z;
                Vector3 position(x, y, z);

                x = mesh->mNormals[i].x;
                y = mesh->mNormals[i].y;
                z = mesh->mNormals[i].z;

                Vector3 normal(x, y, z);

                //We are going to use only uv channel 0 for now
                if (mesh->mTextureCoords[0]) {
                    x = mesh->mTextureCoords[0][i].x;
                    y = mesh->mTextureCoords[0][i].y;
                }
                Vector2 texcoord { x, y };

                verts.push_back(Vertex { position, normal, texcoord });
            }
            for (int i=0; i < mesh->mNumFaces; ++i) {
                polygons.push_back(std::make_shared<Polygon>(verts[mesh->mFaces[i].mIndices[0]],
                                                             verts[mesh->mFaces[i].mIndices[1]],
                                                             verts[mesh->mFaces[i].mIndices[2]]));
                (*(polygons.end() - 1))->SetShader(sh);
            }
        }

        for (int i = 0; i < node->mNumChildren; i++) {
            auto new_polygons = GetAssimpNodePolygons(node->mChildren[i], scene, sh);
            polygons.insert(polygons.end(), new_polygons.begin(), new_polygons.end());
        }
        return polygons;
    }
}
#endif
