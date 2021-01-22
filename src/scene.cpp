#include <scene.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>

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

    this->read_meshes(scene);
    this->read_lights(scene);
    this->read_materials(scene);
    this->read_cameras(scene);
}

void Scene::read_meshes(const aiScene* scene) {
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

            newmesh.tangents.push_back(glm::vec3(
                scene->mMeshes[i]->mTangents[j].x,
                scene->mMeshes[i]->mTangents[j].y,
                scene->mMeshes[i]->mTangents[j].z
            ));

            newmesh.bitangents.push_back(glm::vec3(
                scene->mMeshes[i]->mBitangents[j].x,
                scene->mMeshes[i]->mBitangents[j].y,
                scene->mMeshes[i]->mBitangents[j].z
            ));
        }

        for(uint j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {
            for(uint k = 0; k < 3; k++) {
                newmesh.indices.push_back(scene->mMeshes[i]->mFaces[j].mIndices[k]);
            }
        }

        newmesh.material = scene->mMeshes[i]->mMaterialIndex;

        this->meshes.push_back(newmesh);
    }
}

void Scene::read_lights(const aiScene* scene) {
    for(uint i = 0; i < scene->mNumLights; i++) {
        Light newlight;
        newlight.position = glm::vec3(scene->mLights[i]->mPosition.x,
            scene->mLights[i]->mPosition.y,
            scene->mLights[i]->mPosition.z);
        newlight.color_diffuse = glm::vec3(scene->mLights[i]->mColorDiffuse.r,
            scene->mLights[i]->mColorDiffuse.g,
            scene->mLights[i]->mColorDiffuse.b);
        newlight.color_specular = glm::vec3(scene->mLights[i]->mColorSpecular.r,
            scene->mLights[i]->mColorSpecular.g,
            scene->mLights[i]->mColorSpecular.b);
        newlight.color_ambient = glm::vec3(scene->mLights[i]->mColorAmbient.r,
            scene->mLights[i]->mColorAmbient.g,
            scene->mLights[i]->mColorAmbient.b);
        newlight.attenuation_constant = scene->mLights[i]->mAttenuationConstant;
        newlight.attenuation_linear = scene->mLights[i]->mAttenuationLinear;
        newlight.attenuation_quadratic = scene->mLights[i]->mAttenuationQuadratic;

        this->lights.push_back(newlight);
    }
}

void Scene::read_materials(const aiScene* scene) {
    for(uint i = 0; i < scene->mNumMaterials; i++) {
        Material new_material;

        aiColor4D color;
        aiGetMaterialColor(scene->mMaterials[i], AI_MATKEY_COLOR_DIFFUSE, &color);
        new_material.albedo = glm::vec4(
            color.r, color.g, color.b, color.a
        );
        aiGetMaterialFloat(scene->mMaterials[i], AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, &new_material.metallic);
        aiGetMaterialFloat(scene->mMaterials[i], AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, &new_material.roughness);

        aiString texture_albedo, texture_mr, texture_normal;
        scene->mMaterials[i]->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &texture_albedo);
        scene->mMaterials[i]->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &texture_mr);
        new_material.albedo_texture.data = texture_albedo.data;
        new_material.metallic_texture.data = texture_mr.data;
        new_material.albedo_texture.height = scene->mTextures[atoi(texture_albedo.C_Str())]->mHeight;
        new_material.metallic_texture.height = scene->mTextures[atoi(texture_mr.C_Str())]->mHeight;
        new_material.albedo_texture.width = scene->mTextures[atoi(texture_albedo.C_Str())]->mWidth;
        new_material.metallic_texture.width = scene->mTextures[atoi(texture_mr.C_Str())]->mWidth;

        this->materials.push_back(new_material);
    }
}

void Scene::read_cameras(const aiScene* scene) {
    for(uint i = 0; i < scene->mNumCameras; i++) {
        Camera new_camera;
        new_camera.aspect = scene->mCameras[i]->mAspect;
        new_camera.horizontal_fov = scene->mCameras[i]->mHorizontalFOV;
        new_camera.eye = glm::vec3(
            scene->mCameras[i]->mPosition.x,
            scene->mCameras[i]->mPosition.y,
            scene->mCameras[i]->mPosition.z
        );
        new_camera.target = glm::vec3(
            scene->mCameras[i]->mLookAt.x,
            scene->mCameras[i]->mLookAt.y,
            scene->mCameras[i]->mLookAt.z
        );
        new_camera.eye = glm::vec3(
            scene->mCameras[i]->mUp.x,
            scene->mCameras[i]->mUp.y,
            scene->mCameras[i]->mUp.z
        );

        this->cameras.push_back(new_camera);
        this->current_camera = 0;
    }
}