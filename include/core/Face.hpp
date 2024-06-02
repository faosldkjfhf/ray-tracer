#pragma once

#include <stdint.h>
#include <glm/vec2.hpp>

struct Face {
    uint32_t v0;
    uint32_t v1;
    uint32_t v2;
    uint32_t materialIdx;
    glm::ivec2 textureIndices; // Diffuse, Normal, -1 if no texture
};
