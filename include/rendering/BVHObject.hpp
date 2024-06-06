#pragma once

#include "core/AABB.hpp"

#include "gpumodel/GpuObject.hpp"

#include <glm/glm.hpp>

struct BVHObject {
  glm::vec4 data; // Triangle: v0, v1, v2, 0.0f
                  // Sphere: center, radius
  ObjectType type;
  uint32_t materialIdx;
  glm::ivec2 textureIndices{-1, -1}; // Diffuse, Normal, -1 if no texture
  AABB aabb;
  glm::vec3 centroid;

  operator GpuObject() const {
    return {data, type, materialIdx, textureIndices};
  }
};
