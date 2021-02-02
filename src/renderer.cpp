#include <renderer.hpp>

Renderer::Renderer() {
    image = std::vector<float>(WIDTH * HEIGHT);
}

void Renderer::render(const Scene& scene) {
    instance.build_command_buffer();
    instance.execute_command_buffer(WIDTH, HEIGHT);
    instance.end_command_buffer();
}

void Renderer::save_image() {

}