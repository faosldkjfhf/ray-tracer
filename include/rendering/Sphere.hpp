#pragma once

#include <glm/vec3.hpp>

#include "gpumodel/Material.hpp"

struct Sphere {
  glm::vec3 center{0.0f, 0.0f, 0.0f};
  float radius{1.0f};
  Material material;
};
