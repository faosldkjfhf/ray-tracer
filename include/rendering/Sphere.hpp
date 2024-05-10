#pragma once

#include <glm/vec3.hpp>

struct Sphere {
  glm::vec3 center{0.0f, 0.0f, 0.0f};
  float radius{1.0f};
  alignas(16) int material;
};
