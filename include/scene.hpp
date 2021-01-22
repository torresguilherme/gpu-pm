#pragma once

#include "mesh.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "material.hpp"
#include <vector>
#include <assimp/scene.h>

typedef struct Scene {
    std::vector<Mesh> meshes;
    std::vector<Camera> cameras;
    std::vector<Light> lights;
    std::vector<Material> materials;
    uint current_camera;

    Scene(const char* file_name);
    void read_meshes(const aiScene* scene);
    void read_lights(const aiScene* scene);
    void read_materials(const aiScene* scene);
    void read_cameras(const aiScene* scene);
} Scene;