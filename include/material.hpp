#include <cstdint>
#include <glm/glm.hpp>

typedef struct Material {
    glm::vec4 albedo;
    float roughness;
    float metallic;
    float occlusion_strength;
    int albedo_texture;
    int metallic_texture;
    int normal_map;
    int occlusion_texture;

    Material();
} Material;