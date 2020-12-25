#pragma once

#include <glm/glm.hpp>

typedef struct Light {
    glm::vec3 position;
    float energy;
    float radius;
    float attenuation;

    Light();
} Light;