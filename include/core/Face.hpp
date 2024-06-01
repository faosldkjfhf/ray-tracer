#pragma once
#include <stdint.h>

struct Face {
    uint32_t v0;
    uint32_t v1;
    uint32_t v2;
    uint32_t materialIdx;
    uint32_t gpuTextureIdx;
};
