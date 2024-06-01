#pragma once

#include <vector>

#include "core/Face.hpp"
#include "core/Object.hpp"
#include "rendering/BVH.hpp"
#include "rendering/Sphere.hpp"
#include "rendering/Texture.hpp"
#include "rendering/Shader.hpp"

#include "GpuModel/GpuTexture.hpp"
#include "GpuModel/Material.hpp"
#include "GpuModel/Vertex.hpp"

struct Scene {
  std::vector<Sphere> spheres;
  std::vector<Object> objects;
  std::vector<Material> materials;     // All the materials used in the scene
  std::vector<Texture> textures;       // All the textures used in the scene
  std::vector<GpuTexture> gpuTextures; // Indices to the textures, indexed by object index
  BVH bvh;

  void update();
  void bindTextures(const Shader &shader) const;
  std::vector<Vertex> getVertices() const;
  std::vector<Face> getFaces() const;
};
