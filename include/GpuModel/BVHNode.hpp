#pragma once

#include <cmath>
#include <glm/vec3.hpp>

struct BVHNode {
  alignas(16) glm::vec3 aabbMin{glm::vec3(INFINITY)};
  alignas(16) glm::vec3 aabbMax{glm::vec3(-INFINITY)};
  alignas(4) int leftChild{-1};
  alignas(4) int firstObject{-1};
  alignas(4) int numObjects{0};
};
