#pragma once

#include "core/ObjLoader.hpp"
#include "core/Transform.hpp"

#include "rendering/Material.hpp"
#include "rendering/Mesh.hpp"
#include "rendering/Texture.hpp"

#include <string>

class Object {
public:
  Transform transform;
  Mesh mesh;
  Material material = Material::red();

  Object() = default;
  Object(const Mesh &mesh) : mesh(mesh) {}
  Object(const std::string &filename) {
    std::vector<Texture> textures;
    ObjLoader::loadMesh(filename, mesh, textures);
  }
};
