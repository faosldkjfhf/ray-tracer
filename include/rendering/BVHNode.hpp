#pragma once

#include <cmath>
#include <glm/vec3.hpp>

#include "gpumodel/GpuBvhNode.hpp"

struct BVHNode {
  glm::vec3 aabbMin{glm::vec3(INFINITY)};
  glm::vec3 aabbMax{glm::vec3(-INFINITY)};
  int leftChild{-1};
  int firstObject{-1};
  uint numObjects{0};

  GpuBvhNode toGpuBvhNode() const {
    GpuBvhNode gpuBvhNode;
    gpuBvhNode.aabbMin = aabbMin;
    gpuBvhNode.aabbMax = aabbMax;
    gpuBvhNode.leftChild = leftChild;
    gpuBvhNode.objectIndex = firstObject;
    return gpuBvhNode;
  }
};
