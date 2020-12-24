#include "mesh.hpp"
#include <vector>

typedef struct Scene {
    std::vector<Mesh> meshes;

    Scene(const char* file_name);
} Scene;