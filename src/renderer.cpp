#include <renderer.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Renderer::Renderer() {
    image = std::vector<float>(WIDTH * HEIGHT);
}

void Renderer::render(const Scene& scene) {
    instance.allocate_uniform_data(scene, WIDTH, HEIGHT, SAMPLES_PER_PIXEL);
    instance.build_uniform_buffers(WIDTH, HEIGHT);
    instance.build_descriptor_pool();
    instance.build_descriptor_set();
    instance.send_uniform_data();
    instance.build_command_buffer();
    instance.execute_command_buffer(WIDTH, HEIGHT);
    instance.end_command_buffer();
}

void Renderer::save_image() {
    stbi_write_png("output.png", WIDTH, HEIGHT, 4, this->instance.image.data, 0);
}