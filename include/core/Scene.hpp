#pragma once

#include "core/Object.hpp"
#include "rendering/Material.hpp"
#include "rendering/Sphere.hpp"

#include <vector>

struct Vertex {
  alignas(16) glm::vec3 position;
};

struct Face {
  uint32_t v0;
  uint32_t v1;
  uint32_t v2;
  uint32_t materialIdx;
};

struct Scene {
  std::vector<Sphere> spheres;
  std::vector<Object> objects;
  std::vector<Material> materials;

  void update();
  std::vector<Vertex> getVertices() const;
  std::vector<Face> getFaces() const;
};
