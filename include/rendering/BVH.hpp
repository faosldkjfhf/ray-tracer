#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core/Face.hpp"
#include "core/Vertex.hpp"
#include "rendering/GPUObject.hpp"

struct BVHNode {
  alignas(16) glm::vec3 aabbMin{glm::vec3(INFINITY)};
  alignas(16) glm::vec3 aabbMax{glm::vec3(-INFINITY)};
  alignas(4) int leftChild{-1};
  alignas(4) int firstObject{-1};
  alignas(4) int numObjects{0};
};

class BVH {
 public:
  BVH() = default;

  void buildBVH(const std::vector<GpuObject> &gpuObjects,
                const std::vector<Vertex> &vertices);
  void updateNodeBounds(unsigned int nodeIndex);
  void subdivide(unsigned int nodeIndex);

  const std::vector<BVHNode> &getBVH() const { return _nodes; }
  const std::vector<Vertex> &getVertices() const { return _vertices; }

 private:
  unsigned int _nodesUsed = 1;
  unsigned int _size = 0;
  std::vector<BVHNode> _nodes;

  std::vector<GpuObject> _gpuObjects;
  std::vector<Vertex> _vertices;
};
