#pragma once

#include "core/ObjLoader.hpp"
#include "core/Transform.hpp"

#include "rendering/Mesh.hpp"
#include "rendering/Texture.hpp"

#include <string>

class Object {
public:
  Object() = default;
  Object(const Mesh &mesh) : _mesh(mesh) {}
  Object(const std::string &filename) {
    std::vector<Texture> textures;
    ObjLoader::loadMesh(filename, _mesh, textures);
  }

  Mesh &getMesh() { return _mesh; }
  const Mesh &getMesh() const { return _mesh; }

  Transform &getTransform() { return _transform; }
  const Transform &getTransform() const { return _transform; }

private:
  Transform _transform;

  Mesh _mesh;
};
