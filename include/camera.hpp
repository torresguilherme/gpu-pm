#pragma once

#include <glm/glm.hpp>

typedef struct Camera {
    glm::vec3 eye;
    glm::vec3 target;
    glm::vec3 up;
    float focal_length;
    float aperture;

    Camera();
} Camera;