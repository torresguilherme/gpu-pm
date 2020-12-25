#pragma once

#include <vector>
#include <glm/glm.hpp>

typedef struct Mesh {
    glm::mat4x4 global_transform;
    std::vector<glm::vec3> vertices;
    std::vector<uint> indices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> tex_coords;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec3> bitangents;
    int material;

    Mesh();
} Mesh;