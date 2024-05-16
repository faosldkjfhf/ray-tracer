#include "rendering/BVH.hpp"

#include <algorithm>
#include <iostream>

void BVH::buildBVH(const std::vector<GpuObject> &gpuObjects,
                   const std::vector<Vertex> &vertices) {
  _gpuObjects = gpuObjects;

  _size = _gpuObjects.size();
  _nodes.resize(_size * 2 - 1);

  BVHNode &root = _nodes[0];
  root.firstObject = 0;
  root.leftChild = 0;
  root.numObjects = _size;

  updateNodeBounds(0, vertices);
  subdivide(0, vertices);
  _nodes.resize(_nodesUsed);

  // for (int i = 0; i < _nodes.size(); i++) {
  //   std::cout << "Node " << i << ":\n";
  //   std::cout << "  aabbMin: " << _nodes[i].aabbMin.x << " "
  //             << _nodes[i].aabbMin.y << " " << _nodes[i].aabbMin.z << "\n";
  //   std::cout << "  aabbMax: " << _nodes[i].aabbMax.x << " "
  //             << _nodes[i].aabbMax.y << " " << _nodes[i].aabbMax.z << "\n";
  //   std::cout << "  leftChild: " << _nodes[i].leftChild << "\n";
  //   std::cout << "  firstObject: " << _nodes[i].firstObject << "\n";
  //   std::cout << "  numObjects: " << _nodes[i].numObjects << "\n";
  //   for (int j = _nodes[i].firstObject;
  //        j < _nodes[i].firstObject + _nodes[i].numObjects; j++) {
  //     std::cout << "    Object " << j << ":\n";
  //     if (_gpuObjects[j].type == GpuObjectType::Face) {
  //       std::cout << "      Face: " << _gpuObjects[j].data.x << " "
  //                 << _gpuObjects[j].data.y << " " << _gpuObjects[j].data.z
  //                 << "\n";
  //     } else {
  //       std::cout << "      Sphere: " << _gpuObjects[j].data.x << " "
  //                 << _gpuObjects[j].data.y << " " << _gpuObjects[j].data.z
  //                 << " " << _gpuObjects[j].data.w << "\n";
  //     }
  //   }
  // }
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
  if (node.numObjects <= 2)
    return;
  glm::vec3 extent = node.aabbMax - node.aabbMin;
  int axis = 0;
  if (extent.y > extent.x)
    axis = 1;
  if (extent.z > extent[axis])
    axis = 2;
  float splitPos = node.aabbMin[axis] + extent[axis] * 0.5f;

  int i = node.firstObject;
  int j = node.firstObject + node.numObjects - 1;
  while (i <= j) {
    const auto &object = _gpuObjects[i];
    glm::vec3 centroid;
    if (object.type == GpuObjectType::Face) {
      glm::vec3 v0 = vertices[object.data.x].position;
      glm::vec3 v1 = vertices[object.data.y].position;
      glm::vec3 v2 = vertices[object.data.z].position;
      centroid = (v0 + v1 + v2) / 3.0f;
    } else {
      centroid = glm::vec3(object.data.x, object.data.y, object.data.z);
    }
    if (centroid[axis] < splitPos) {
      std::swap(_gpuObjects.at(i), _gpuObjects.at(j--));
    } else {
      i++;
    }
  }

  int leftCount = i - node.firstObject;
  if (leftCount == 0 || leftCount == node.numObjects) {
    return;
  }

  int leftIndex = _nodesUsed++;
  int rightIndex = _nodesUsed++;

  _nodes[leftIndex].firstObject = node.firstObject;
  _nodes[leftIndex].numObjects = leftCount;

  _nodes[rightIndex].firstObject = i;
  _nodes[rightIndex].numObjects = node.numObjects - leftCount;

  node.leftChild = leftIndex;
  node.numObjects = 0;

  updateNodeBounds(leftIndex, vertices);
  updateNodeBounds(rightIndex, vertices);
  subdivide(leftIndex, vertices);
  subdivide(rightIndex, vertices);
}

AABB BVH::getAABB(GpuObject &object,
                  const std::vector<Vertex> &vertices) const {
  if (object.type == GpuObjectType::Face) {
    glm::vec3 v0 = vertices[object.data.x].position;
    glm::vec3 v1 = vertices[object.data.y].position;
    glm::vec3 v2 = vertices[object.data.z].position;
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);

    min = glm::min(v0, glm::min(v1, v2));
    max = glm::max(v0, glm::max(v1, v2));
    return {min, max};
  } else /*  if (object.type == GpuObjectType::Sphere) */ {
    glm::vec3 radius = glm::vec3(object.data.w);
    glm::vec3 center = glm::vec3(object.data.x, object.data.y, object.data.z);
    return {center - radius, center + radius};
  }
}
