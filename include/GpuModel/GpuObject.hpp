#pragma once

#include <glm/glm.hpp>

enum class GpuObjectType { Face, Sphere };

struct GpuObject {
  alignas(16) glm::vec4 data; // Triangle: v0, v1, v2, 0.0f
                              // Sphere: center, radius
  alignas(4) GpuObjectType type;
  alignas(4) uint32_t materialIdx;
  alignas(4) uint32_t gpuTextureIdx;
};
