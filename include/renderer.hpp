#include <scene.hpp>
#include <gpu_instance.hpp>
#include <vector>

const uint WIDTH = 640;
const uint HEIGHT = 480;

typedef struct Renderer {
    GPUInstance instance;

    void render(const Scene& scene);
    Renderer();
} Renderer;