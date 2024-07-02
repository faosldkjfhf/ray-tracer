#include "rendering/BVH.hpp"

#include <algorithm>
#include <iostream>

void BVH::buildBVH(const std::vector<GpuObject> &gpuObjects,
                   const std::vector<Vertex> &vertices) {
  // Convert GpuObjects to BVHObjects
  _objects.resize(gpuObjects.size());
  for (int i = 0; i < gpuObjects.size(); i++) {
    const auto &gpuObject = gpuObjects[i];
    _objects[i] = {gpuObject.data,
                   gpuObject.type,
                   gpuObject.materialIdx,
                   gpuObject.textureIndices,
                   getAABB(gpuObject, vertices),
                   getCentroid(gpuObject, vertices)};
  }

  auto size = _objects.size();
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
    AABB aabb = _objects[i].aabb;
    node.aabbMin = glm::min(node.aabbMin, aabb.min);
    node.aabbMax = glm::max(node.aabbMax, aabb.max);
  }
}

void BVH::subdivide(unsigned int nodeIndex,
                    const std::vector<Vertex> &vertices) {
  BVHNode &node = _nodes.at(nodeIndex);

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
    if (_objects[i].centroid[splitAxis] < splitPos) {
      i++;
    } else {
      std::swap(_objects[i], _objects[j--]);
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

float BVH::findBestSplit(const BVHNode &node, int &splitAxis, float &splitPos,
                         const std::vector<Vertex> &vertices) const {
  // Find centroid bounds
  AABB bounds;
  for (int i = node.leftFirst; i < node.leftFirst + node.numObjects; i++) {
    bounds.extend(_objects[i].centroid);
  }

  float bestCost = INFINITY;
  for (int axis = 0; axis < 3; axis++) {
    float min = bounds.min[axis];
    float max = bounds.max[axis];
    if (min == max)
      continue;

    // Calculate the bins
    Bin bins[BIN_COUNT];
    float scale = BIN_COUNT / (max - min);
    for (int i = node.leftFirst; i < node.leftFirst + node.numObjects; i++) {
      const auto &object = _objects[i];
      int binIdx = std::min(BIN_COUNT - 1,
                            (uint)((object.centroid[axis] - min) * scale));
      bins[binIdx].aabb.extend(object.aabb);
      bins[binIdx].numObjects++;
    }

    // Calculate number of objects and area for each bin
    float leftArea[BIN_COUNT - 1], rightArea[BIN_COUNT - 1];
    int leftCount[BIN_COUNT - 1], rightCount[BIN_COUNT - 1];
    AABB leftAABB, rightAABB;
    int leftSum = 0, rightSum = 0;
    for (int i = 0; i < BIN_COUNT - 1; i++) {
      leftSum += bins[i].numObjects;
      leftCount[i] = leftSum;
      leftAABB.extend(bins[i].aabb);
      leftArea[i] = leftAABB.surfaceArea();

      rightSum += bins[BIN_COUNT - 1 - i].numObjects;
      rightCount[BIN_COUNT - 2 - i] = rightSum;
      rightAABB.extend(bins[BIN_COUNT - 1 - i].aabb);
      rightArea[BIN_COUNT - 2 - i] = rightAABB.surfaceArea();
    }

    // Evaluate SAH for each split
    scale = (max - min) / BIN_COUNT;
    for (int i = 0; i < BIN_COUNT - 1; i++) {
      float cost = leftArea[i] * leftCount[i] + rightArea[i] * rightCount[i];
      if (cost < bestCost) {
        bestCost = cost;
        splitAxis = axis;
        splitPos = min + (i + 1) * scale;
      }
    }
  }

  return bestCost;
}

AABB BVH::getAABB(const GpuObject &object,
                  const std::vector<Vertex> &vertices) const {
  if (object.type == ObjectType::Face) {
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
  if (object.type == ObjectType::Face) {
    glm::vec3 v0 = vertices[object.data.x].position;
    glm::vec3 v1 = vertices[object.data.y].position;
    glm::vec3 v2 = vertices[object.data.z].position;
    return (v0 + v1 + v2) / 3.0f;
  } else /*  if (object.type == GpuObjectType::Sphere) */ {
    return glm::vec3(object.data.x, object.data.y, object.data.z);
  }
}

std::vector<GpuObject> BVH::getGpuObjects() const {
  // Convert from bvh to gpu
  std::vector<GpuObject> gpuObjects(_objects.size());
  for (int i = 0; i < _objects.size(); i++) {
    const auto &bvhObject = _objects[i];
    gpuObjects[i] = bvhObject;
  }
  return gpuObjects;
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
      if (bvh._objects[j].type == ObjectType::Face) {
        os << "      Face: " << bvh._objects[j].data.x << " "
           << bvh._objects[j].data.y << " " << bvh._objects[j].data.z << "\n";
      } else {
        os << "      Sphere: " << bvh._objects[j].data.x << " "
           << bvh._objects[j].data.y << " " << bvh._objects[j].data.z << " "
           << bvh._objects[j].data.w << "\n";
      }
    }
  }
  return os;
}
