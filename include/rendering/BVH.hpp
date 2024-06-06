#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "core/AABB.hpp"
#include "rendering/BVHNode.hpp"

#include "gpumodel/GpuObject.hpp"
#include "gpumodel/Vertex.hpp"

#include <iostream>

class BVH {
public:
  BVH() = default;

  void buildBVH(const std::vector<GpuObject> &gpuObjects,
                const std::vector<Vertex> &vertices);
  void updateNodeBounds(unsigned int nodeIndex,
                        const std::vector<Vertex> &vertices);
  void subdivide(unsigned int nodeIndex, const std::vector<Vertex> &vertices);
  float evaluateSAH(const BVHNode &node, int axis, float pos,
                    const std::vector<Vertex> &vertices) const;
  float findBestSplit(const BVHNode &node, int &splitAxis, float &splitPos,
                      const std::vector<Vertex> &vertices) const;

  AABB getAABB(const GpuObject &object,
               const std::vector<Vertex> &vertices) const;
  glm::vec3 getCentroid(const GpuObject &object,
                        const std::vector<Vertex> &vertices) const;

  friend std::ostream &operator<<(std::ostream &os, const BVH &bvh);

  const std::vector<BVHNode> &getNodes() const { return _nodes; }
  const std::vector<GpuObject> &getGpuObjects() const { return _gpuObjects; }

private:
  unsigned int _nodesUsed = 1;
  std::vector<BVHNode> _nodes;
  std::vector<GpuObject> _gpuObjects;
};
