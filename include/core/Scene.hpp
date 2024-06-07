#pragma once

#include <vector>

#include "core/Face.hpp"
#include "core/Object.hpp"
#include "rendering/BVH.hpp"
#include "rendering/Sphere.hpp"
#include "rendering/Texture.hpp"

#include "gpumodel/Material.hpp"
#include "gpumodel/Vertex.hpp"

struct Scene {
  std::vector<Sphere> spheres;
  std::vector<Object> objects;
  std::vector<Material> materials;     // All the materials used in the scene
  std::vector<Texture> textures;       // All the textures used in the scene
  BVH bvh;

  void update();
  std::vector<Vertex> getVertices() const;
  std::vector<Face> getFaces() const;
};
