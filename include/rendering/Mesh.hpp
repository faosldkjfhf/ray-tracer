#pragma once

#include <vector>

#include "rendering/MeshVertex.hpp"

struct Mesh {
  std::vector<MeshVertex> _vertices;
  std::vector<unsigned int> _indices;
};
