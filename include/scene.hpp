#pragma once

#include "mesh.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "texture.hpp"
#include "material.hpp"
#include <vector>

typedef struct Scene {
    std::vector<Mesh> meshes;
    std::vector<Camera> cameras;
    std::vector<Light> lights;
    std::vector<Texture> textures;
    std::vector<Material> materials;

    Scene(const char* file_name);
} Scene;