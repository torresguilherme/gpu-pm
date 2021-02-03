#include <renderer.hpp>

Renderer::Renderer() {
    image = std::vector<float>(WIDTH * HEIGHT);
}

void Renderer::render(const Scene& scene) {
    instance.allocate_uniform_data(scene, WIDTH, HEIGHT, SAMPLES_PER_PIXEL);
    instance.send_uniform_data();
    instance.build_command_buffer();
    instance.execute_command_buffer(WIDTH, HEIGHT);
    instance.end_command_buffer();
}

void Renderer::save_image() {

}