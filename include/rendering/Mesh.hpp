#pragma once

#include <vector>

#include "rendering/MeshVertex.hpp"

struct Mesh {
  std::vector<MeshVertex> vertices;
  std::vector<unsigned int> indices;
};
