#pragma once

#include <vector>
#include <glm/glm.hpp>

typedef struct Mesh {
    glm::mat4x4 global_transform;
    std::vector<glm::vec4> vertices;
    std::vector<uint> indices;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> tex_coords;
    std::vector<glm::vec4> tangents;
    std::vector<glm::vec4> bitangents;
    int material;

    Mesh();
} Mesh;