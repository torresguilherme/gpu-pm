#pragma once

#include <vector>
#include <cstdint>

typedef struct Texture {
    char* data;
    size_t height;
    size_t width;

    Texture();
} Texture;
