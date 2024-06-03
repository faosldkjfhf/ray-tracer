#pragma once

#include <glm/glm.hpp>

struct AABB {
  glm::vec3 min{INFINITY};
  glm::vec3 max{-INFINITY};
  void extend(const glm::vec3 &point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
  }
  void extend(const AABB &aabb) {
    min = glm::min(min, aabb.min);
    max = glm::max(max, aabb.max);
  }
  float surfaceArea() const {
    glm::vec3 extent = max - min;
    return extent.x * extent.y + extent.y * extent.z + extent.z * extent.x;
  }
};
