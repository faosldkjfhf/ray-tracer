#pragma once

#include "core/Material.hpp"
#include "core/Object.hpp"
#include "core/Sphere.hpp"
#include "rendering/Triangle.hpp"
#include <vector>

struct Scene {
  std::vector<Sphere> spheres;
  // std::vector<Object> objects;
  std::vector<Triangle> triangles;
  std::vector<Material> materials;
};
