#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core/AABB.hpp"
#include "core/Vertex.hpp"
#include "rendering/GpuObject.hpp"

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
  void updateNodeBounds(unsigned int nodeIndex,
                        const std::vector<Vertex> &vertices);
  void subdivide(unsigned int nodeIndex, const std::vector<Vertex> &vertices);

  AABB getAABB(const GpuObject &object,
               const std::vector<Vertex> &vertices) const;

  const std::vector<BVHNode> &getBVHNodes() const { return _nodes; }
  const std::vector<GpuObject> &getGpuObjects() const { return _gpuObjects; }

private:
  unsigned int _nodesUsed = 1;
  std::vector<BVHNode> _nodes;
  std::vector<GpuObject> _gpuObjects;
};
