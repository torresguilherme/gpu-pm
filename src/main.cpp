#include <scene.hpp>
#include <renderer.hpp>
#include <cstdio>

int main(int argc, char** argv) {
    if(argc == 1) {
        printf("Usage: ./demo <scene file name>\n");
        return -1;
    }

    printf("Initializing renderer...\n");
    Renderer renderer = Renderer();
    printf("Initializing scene...\n");
    Scene scene = Scene(argv[1]);
    printf("Rendering...\n");
    renderer.render(scene);
    printf("Saving image...\n");
    renderer.save_image();
    return 0;
}