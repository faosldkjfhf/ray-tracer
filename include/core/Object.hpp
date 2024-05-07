#pragma once

#include "glm/vec3.hpp"

#include "core/Transform.hpp"

#include "rendering/Mesh.hpp"

class Object {
public:
  Object() = default;

private:
  Transform _transform;

  Mesh _mesh;
};
