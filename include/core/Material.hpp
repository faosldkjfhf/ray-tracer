#pragma once
#include <glm/glm.hpp>

enum class MaterialType {
  LAMBERTIAN,
  METAL,
  GLASS,
  LIGHT
};

struct Material {
  alignas(16) glm::vec3 color;
  MaterialType type;
  // alignas(16) glm::vec3 emissionColor;
  // float emissionStrength;
};
