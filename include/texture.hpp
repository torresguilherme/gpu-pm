#pragma once

#include <vector>
#include <cstdint>

typedef struct Texture {
    char* data;
    int height;
    int width;

    Texture();
} Texture;
