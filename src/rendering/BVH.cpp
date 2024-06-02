#include "rendering/BVH.hpp"

#include <algorithm>
#include <iostream>

void BVH::buildBVH(const std::vector<GpuObject> &gpuObjects,
                   const std::vector<Vertex> &vertices) {
  _gpuObjects = gpuObjects;

  auto size = _gpuObjects.size();
  _nodes.resize(size * 2 - 1);

  BVHNode &root = _nodes[0];
  root.leftChild = 0;
  root.firstObject = 0;
  root.numObjects = size;

  updateNodeBounds(0, vertices);
  subdivide(0, vertices);
  _nodes.resize(_nodesUsed);
}

void BVH::updateNodeBounds(unsigned int nodeIndex,
                           const std::vector<Vertex> &vertices) {
  BVHNode &node = _nodes[nodeIndex];
  for (int i = node.firstObject; i < node.firstObject + node.numObjects; i++) {
    AABB aabb = getAABB(_gpuObjects[i], vertices);
    node.aabbMin = glm::min(node.aabbMin, aabb.min);
    node.aabbMax = glm::max(node.aabbMax, aabb.max);
  }
}

void BVH::subdivide(unsigned int nodeIndex,
                    const std::vector<Vertex> &vertices) {
  BVHNode &node = _nodes[nodeIndex];

  if (node.numObjects <= 1)
    return;

  glm::vec3 extent = node.aabbMax - node.aabbMin;
  int axis = 0;
  if (extent.y > extent.x)
    axis = 1;
  if (extent.z > extent[axis])
    axis = 2;

  // Sort objects along axis
  std::sort(_gpuObjects.begin() + node.firstObject,
            _gpuObjects.begin() + node.firstObject + node.numObjects,
            [&](const GpuObject &a, const GpuObject &b) {
              return getAABB(a, vertices).min[axis] <
                     getAABB(b, vertices).min[axis];
            });

  int leftCount = node.numObjects / 2;

  int leftIndex = _nodesUsed++;
  int rightIndex = _nodesUsed++;

  _nodes[leftIndex].firstObject = node.firstObject;
  _nodes[leftIndex].numObjects = leftCount;

  _nodes[rightIndex].firstObject = node.firstObject + leftCount;
  _nodes[rightIndex].numObjects = node.numObjects - leftCount;

  node.leftChild = leftIndex;
  node.firstObject = -1;
  node.numObjects = 0;

  updateNodeBounds(leftIndex, vertices);
  updateNodeBounds(rightIndex, vertices);
  subdivide(leftIndex, vertices);
  subdivide(rightIndex, vertices);
}

AABB BVH::getAABB(const GpuObject &object,
                  const std::vector<Vertex> &vertices) const {
  if (object.type == GpuObjectType::Face) {
    glm::vec3 v0 = vertices[object.data.x].position;
    glm::vec3 v1 = vertices[object.data.y].position;
    glm::vec3 v2 = vertices[object.data.z].position;
    glm::vec3 min = glm::min(v0, glm::min(v1, v2));
    glm::vec3 max = glm::max(v0, glm::max(v1, v2));
    return {min, max};
  } else /*  if (object.type == GpuObjectType::Sphere) */ {
    glm::vec3 radius = glm::vec3(object.data.w);
    glm::vec3 center = glm::vec3(object.data.x, object.data.y, object.data.z);
    return {center - radius, center + radius};
  }
}

std::ostream &operator<<(std::ostream &os, const BVH &bvh) {
  for (int i = 0; i < bvh._nodes.size(); i++) {
    os << "Node " << i << ":\n";
    os << "  aabbMin: " << bvh._nodes[i].aabbMin.x << " "
       << bvh._nodes[i].aabbMin.y << " " << bvh._nodes[i].aabbMin.z << "\n";
    os << "  aabbMax: " << bvh._nodes[i].aabbMax.x << " "
       << bvh._nodes[i].aabbMax.y << " " << bvh._nodes[i].aabbMax.z << "\n";
    os << "  leftChild: " << bvh._nodes[i].leftChild << "\n";
    os << "  firstObject: " << bvh._nodes[i].firstObject << "\n";
    os << "  numObjects: " << bvh._nodes[i].numObjects << "\n";
    for (int j = bvh._nodes[i].firstObject;
         j < bvh._nodes[i].firstObject + bvh._nodes[i].numObjects; j++) {
      os << "    Object " << j << ":\n";
      if (bvh._gpuObjects[j].type == GpuObjectType::Face) {
        os << "      Face: " << bvh._gpuObjects[j].data.x << " "
           << bvh._gpuObjects[j].data.y << " " << bvh._gpuObjects[j].data.z
           << "\n";
      } else {
        os << "      Sphere: " << bvh._gpuObjects[j].data.x << " "
           << bvh._gpuObjects[j].data.y << " " << bvh._gpuObjects[j].data.z
           << " " << bvh._gpuObjects[j].data.w << "\n";
      }
    }
  }
  return os;
}
