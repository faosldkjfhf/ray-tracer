#pragma once

#include <glm/glm.hpp>

enum class GpuObjectType { Face, Sphere };

struct GpuObject {
  alignas(16) glm::vec4 data; // Triangle: v0, v1, v2, 0.0f
                              // Sphere: center, radius
  GpuObjectType type;
  uint32_t materialIdx;
  glm::ivec2 textureIndices; // Diffuse, Normal, -1 if no texture
};
