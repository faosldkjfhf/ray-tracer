#pragma once

#include <cmath>
#include <glm/vec3.hpp>

struct alignas(32) BVHNode {
  glm::vec3 aabbMin{glm::vec3(INFINITY)};
  uint leftFirst{0}; // Left child or first object
  glm::vec3 aabbMax{glm::vec3(-INFINITY)};
  uint numObjects{0};
};
