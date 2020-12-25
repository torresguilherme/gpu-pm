#pragma once

#include <glm/glm.hpp>

typedef struct Light {
    glm::vec3 position;
    glm::vec3 color_diffuse;
    glm::vec3 color_specular;
    glm::vec3 color_ambient;
    float attenuation_constant;
    float attenuation_linear;
    float attenuation_quadratic; 

    Light();
} Light;