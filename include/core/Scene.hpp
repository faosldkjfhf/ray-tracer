#pragma once

#include "core/Object.hpp"
#include "rendering/Material.hpp"
#include "rendering/Sphere.hpp"

#include <vector>

struct Vertex {
  alignas(16) glm::vec3 position;
};

struct Face {
  unsigned int v0;
  unsigned int v1;
  unsigned int v2;
  unsigned int materialIdx;
};

struct Scene {
  std::vector<Sphere> spheres;
  std::vector<Object> objects;
  std::vector<Material> materials;

  void update();
  std::vector<Vertex> getVertices() const;
  std::vector<Face> getFaces() const;
};
