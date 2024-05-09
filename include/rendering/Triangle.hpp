#pragma once

#include <glm/vec3.hpp>

struct Triangle
{
  // glm::vec3 v0;
  // float pad0;
  // glm::vec3 v1;
  // float pad1;
  // glm::vec3 v2;
  // float pad2;
  alignas(16) glm::vec3 v0;
  alignas(16) glm::vec3 v1;
  alignas(16) glm::vec3 v2;
  int material;
};
