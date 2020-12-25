#include <scene.hpp>
#include <cstdio>
#include <assimp/scene.h>

int main(int argc, char** argv) {
    if(argc == 1) {
        printf("Usage: ./demo <scene file name>\n");
        return -1;
    }

    Scene scene = Scene(argv[1]);
}