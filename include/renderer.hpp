#include <scene.hpp>
#include <gpu_instance.hpp>
#include <vector>

const uint WIDTH = 640;
const uint HEIGHT = 480;
const uint SAMPLES_PER_PIXEL = 400;

typedef struct Renderer {
    GPUInstance instance;
    std::vector<float> image;

    void render(const Scene& scene);
    void save_image();
    Renderer();
} Renderer;