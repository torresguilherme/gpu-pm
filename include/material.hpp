#include "texture.hpp"
#include <cstdint>
#include <glm/glm.hpp>

typedef struct Material {
    glm::vec4 albedo;
    float roughness;
    float metallic;
    Texture albedo_texture;
    Texture metallic_texture;

    Material();
} Material;