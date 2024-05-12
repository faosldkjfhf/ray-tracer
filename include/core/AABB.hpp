#pragma once

#include <glm/vec3.hpp>

struct AABB {
  alignas(16) glm::vec3 min{0.0f};
  alignas(16) glm::vec3 max{0.0f};
};
