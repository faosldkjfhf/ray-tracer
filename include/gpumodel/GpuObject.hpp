#pragma once

#include <glm/glm.hpp>

enum class ObjectType { Face, Sphere };

struct GpuObject {
  alignas(16) glm::vec4 data; // Triangle: v0, v1, v2, 0.0f
                              // Sphere: center, radius
  ObjectType type;
  uint32_t materialIdx;
  glm::ivec2 textureIndices{-1, -1}; // Diffuse, Normal, -1 if no texture
};
