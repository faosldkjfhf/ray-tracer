#include "rendering/BVH.hpp"

#include <algorithm>
#include <iostream>

const uint MIN_OBJECTS = 2;

void BVH::buildBVH(const std::vector<GpuObject> &gpuObjects,
                   const std::vector<Vertex> &vertices) {
  _gpuObjects = gpuObjects;

  auto size = _gpuObjects.size();
  _nodes.resize(size * 2 - 1);

  BVHNode &root = _nodes[0];
  root.leftFirst = 0;
  root.numObjects = size;

  updateNodeBounds(0, vertices);
  subdivide(0, vertices);
  _nodes.resize(_nodesUsed);
}

void BVH::updateNodeBounds(unsigned int nodeIndex,
                           const std::vector<Vertex> &vertices) {
  BVHNode &node = _nodes[nodeIndex];
  for (int i = node.leftFirst; i < node.leftFirst + node.numObjects; i++) {
    AABB aabb = getAABB(_gpuObjects[i], vertices);
    node.aabbMin = glm::min(node.aabbMin, aabb.min);
    node.aabbMax = glm::max(node.aabbMax, aabb.max);
  }
}

void BVH::subdivide(unsigned int nodeIndex,
                    const std::vector<Vertex> &vertices) {
  BVHNode &node = _nodes[nodeIndex];

  if (node.numObjects <= MIN_OBJECTS) {
    return;
  }

  // Find best split axis and position
  int splitAxis = 0;
  float splitPos = 0.0f;
  float bestCost = findBestSplit(node, splitAxis, splitPos, vertices);

  // Check if subdivision is worth it
  AABB nodeAABB{node.aabbMin, node.aabbMax};
  float nodeArea = nodeAABB.surfaceArea();
  if (bestCost >= node.numObjects * nodeArea) {
    return;
  }

  // Partition objects
  int i = node.leftFirst;
  int j = i + node.numObjects - 1;
  while (i <= j) {
    if (getCentroid(_gpuObjects[i], vertices)[splitAxis] < splitPos) {
      i++;
    } else {
      std::swap(_gpuObjects[i], _gpuObjects[j--]);
    }
  }

  int leftCount = i - node.leftFirst;

  // Create left and right child nodes
  int leftIndex = _nodesUsed++;
  int rightIndex = _nodesUsed++;

  _nodes[leftIndex].leftFirst = node.leftFirst;
  _nodes[leftIndex].numObjects = leftCount;

  _nodes[rightIndex].leftFirst = i;
  _nodes[rightIndex].numObjects = node.numObjects - leftCount;

  node.leftFirst = leftIndex;
  node.numObjects = 0;

  updateNodeBounds(leftIndex, vertices);
  updateNodeBounds(rightIndex, vertices);
  subdivide(leftIndex, vertices);
  subdivide(rightIndex, vertices);
}

float BVH::evaluateSAH(const BVHNode &node, int axis, float pos,
                       const std::vector<Vertex> &vertices) const {
  AABB leftAABB, rightAABB;
  int leftCount = 0, rightCount = 0;

  for (int i = node.leftFirst; i < node.leftFirst + node.numObjects; i++) {
    glm::vec3 centroid = getCentroid(_gpuObjects[i], vertices);
    if (centroid[axis] < pos) {
      leftAABB.extend(getAABB(_gpuObjects[i], vertices));
      leftCount++;
    } else {
      rightAABB.extend(getAABB(_gpuObjects[i], vertices));
      rightCount++;
    }
  }

  float cost =
      leftCount * leftAABB.surfaceArea() + rightCount * rightAABB.surfaceArea();
  return cost > 0.0f ? cost : INFINITY;
}

float BVH::findBestSplit(const BVHNode &node, int &splitAxis, float &splitPos,
                         const std::vector<Vertex> &vertices) const {
  float bestCost = INFINITY;
  for (int axis = 0; axis < 3; axis++) {
    float min = node.aabbMin[axis];
    float max = node.aabbMax[axis];
    if (min == max)
      continue;
    float scale = (max - min) / 100;
    for (int i = 1; i < 100; i++) {
      float pos = min + i * scale;
      float cost = evaluateSAH(node, axis, pos, vertices);
      if (cost < bestCost) {
        bestCost = cost;
        splitAxis = axis;
        splitPos = pos;
      }
    }
  }

  return bestCost;
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

glm::vec3 BVH::getCentroid(const GpuObject &object,
                           const std::vector<Vertex> &vertices) const {
  if (object.type == GpuObjectType::Face) {
    glm::vec3 v0 = vertices[object.data.x].position;
    glm::vec3 v1 = vertices[object.data.y].position;
    glm::vec3 v2 = vertices[object.data.z].position;
    return (v0 + v1 + v2) / 3.0f;
  } else /*  if (object.type == GpuObjectType::Sphere) */ {
    return glm::vec3(object.data.x, object.data.y, object.data.z);
  }
}

std::ostream &operator<<(std::ostream &os, const BVH &bvh) {
  std::cout << "sizeof(BVHNode): " << sizeof(BVHNode) << "\n";
  for (int i = 0; i < bvh._nodes.size(); i++) {
    os << "Node " << i << ":\n";
    os << "  aabbMin: " << bvh._nodes[i].aabbMin.x << " "
       << bvh._nodes[i].aabbMin.y << " " << bvh._nodes[i].aabbMin.z << "\n";
    os << "  aabbMax: " << bvh._nodes[i].aabbMax.x << " "
       << bvh._nodes[i].aabbMax.y << " " << bvh._nodes[i].aabbMax.z << "\n";
    os << "  leftFirst: " << bvh._nodes[i].leftFirst << "\n";
    os << "  numObjects: " << bvh._nodes[i].numObjects << "\n";
    for (int j = bvh._nodes[i].leftFirst;
         j < bvh._nodes[i].leftFirst + bvh._nodes[i].numObjects; j++) {
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
