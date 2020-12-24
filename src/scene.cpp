#include <scene.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Scene::Scene(const char* file_name) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        file_name,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType
    );

    if (!scene) {
        printf("Couldn't import scene: %s. Error: %s\n", file_name, importer.GetErrorString());
        exit(1);
    }

    for(uint i = 0; i < scene->mNumMeshes; i++) {
        Mesh newmesh;
        for (uint j = 0; j < scene->mMeshes[i]->mVertices->Length(); j++) {
            newmesh.vertices.push_back({
                scene->mMeshes[i]->mVertices[j].x,
                scene->mMeshes[i]->mVertices[j].y,
                scene->mMeshes[i]->mVertices[j].z   
            });

            newmesh.normals.push_back({
                scene->mMeshes[i]->mNormals[j].x,
                scene->mMeshes[i]->mNormals[j].y,
                scene->mMeshes[i]->mNormals[j].z
            });

            newmesh.tex_coords.push_back(glm::vec2(
                scene->mMeshes[i]->mTextureCoords[j]->x,
                scene->mMeshes[i]->mTextureCoords[j]->y
            ));
        }
    }
}