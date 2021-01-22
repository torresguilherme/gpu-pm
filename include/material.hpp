#include <cstdint>
#include <glm/glm.hpp>

typedef struct Material {
    glm::vec4 albedo;
    float roughness;
    float metallic;
    char* albedo_texture;
    char* metallic_texture;

    Material();
} Material;