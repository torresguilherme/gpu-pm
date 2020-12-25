#pragma once

#include <vector>
#include <cstdint>

typedef struct Texture {
    std::vector<std::vector<uint8_t>> data;
    size_t width;
    size_t height;

    Texture();
} Texture;
