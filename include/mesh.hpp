#pragma once

#include <vector>
#include <glm/glm.hpp>

typedef struct Mesh {
    glm::mat4x4 global_transform;
    std::vector<glm::vec3[3]> triangles;
    std::vector<glm::vec3[3]> normals;
    std::vector<glm::vec3[3]> tex_coords;
    std::vector<glm::vec3[3]> tangents;
    std::vector<glm::vec3[3]> bitangents;

    Mesh(const char* file_name);
} Mesh;