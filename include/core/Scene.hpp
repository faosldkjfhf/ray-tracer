#pragma once

#include <vector>

#include "core/Face.hpp"
#include "core/Object.hpp"
#include "core/Vertex.hpp"
#include "rendering/BVH.hpp"
#include "rendering/GPUObject.hpp"
#include "rendering/Material.hpp"
#include "rendering/Sphere.hpp"

struct Scene {
  // std::vector<Sphere> spheres;
  std::vector<Object> objects;
  std::vector<Material> materials;

  std::vector<GpuObject> gpuObjects;
  BVH bvh;

  void update();
  std::vector<Vertex> getVertices() const;
  std::vector<Face> getFaces() const;
};
