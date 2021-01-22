#include <scene.hpp>
#include <renderer.hpp>
#include <cstdio>
#include <assimp/scene.h>

int main(int argc, char** argv) {
    if(argc == 1) {
        printf("Usage: ./demo <scene file name>\n");
        return -1;
    }

    printf("Initializing scene...\n");
    Scene scene = Scene(argv[1]);
    printf("Initializing renderer...\n");
    Renderer renderer = Renderer();
}