#pragma once

#include <cmath>
#include <glm/vec3.hpp>

struct GpuBvhNode {
  glm::vec3 aabbMin{glm::vec3(INFINITY)};
  int leftChild{-1};
  glm::vec3 aabbMax{glm::vec3(-INFINITY)};
  int objectIndex{-1};
};
