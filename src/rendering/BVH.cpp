#include "rendering/BVH.hpp"

#include <algorithm>

void BVH::buildBVH(const std::vector<GpuObject> &gpuObjects,
                   const std::vector<Vertex> &vertices) {
  _gpuObjects = gpuObjects;
  _vertices = vertices;

  _size = _gpuObjects.size();
  _nodes.resize(_size * 2 - 1);

  BVHNode &root = _nodes[0];
  root.firstObject = 0;
  root.leftChild = 0;
  root.numObjects = _size;

  updateNodeBounds(0);
  subdivide(0);
}

void BVH::updateNodeBounds(unsigned int nodeIndex) {
  BVHNode &node = _nodes[nodeIndex];
  for (int i = node.firstObject; i < node.firstObject + node.numObjects; i++) {
    GpuObject &object = _gpuObjects[i];
    if (object.type == 0) {
      glm::vec3 radius = glm::vec3(object.data.w);
      glm::vec3 center = glm::vec3(object.data.x, object.data.y, object.data.z);
      node.aabbMin.x = fminf(node.aabbMin.x, center.x - radius.x);
      node.aabbMin.y = fminf(node.aabbMin.y, center.y - radius.y);
      node.aabbMin.z = fminf(node.aabbMin.z, center.z - radius.z);

      node.aabbMax.x = fmaxf(node.aabbMax.x, center.x - radius.x);
      node.aabbMax.y = fmaxf(node.aabbMax.y, center.y - radius.y);
      node.aabbMax.z = fmaxf(node.aabbMax.z, center.z - radius.z);

    } else if (object.type == 1) {
      glm::vec3 v0 = _vertices[object.data.x].position;
      glm::vec3 v1 = _vertices[object.data.y].position;
      glm::vec3 v2 = _vertices[object.data.z].position;
      node.aabbMin.x = fminf(node.aabbMin.x, fminf(fminf(v0.x, v1.x), v2.x));
      node.aabbMin.z = fminf(node.aabbMin.z, fminf(fminf(v0.z, v1.z), v2.z));
      node.aabbMin.y = fminf(node.aabbMin.y, fminf(fminf(v0.y, v1.y), v2.y));

      node.aabbMax.x = fmaxf(node.aabbMax.x, fmaxf(fmaxf(v0.x, v1.x), v2.x));
      node.aabbMax.y = fmaxf(node.aabbMax.y, fmaxf(fmaxf(v0.y, v1.y), v2.y));
      node.aabbMax.z = fmaxf(node.aabbMax.z, fmaxf(fmaxf(v0.z, v1.z), v2.z));
    }
  }
}

void BVH::subdivide(unsigned int nodeIndex) {
  BVHNode &node = _nodes[nodeIndex];
  if (node.numObjects <= 2) return;
  glm::vec3 extent = node.aabbMax - node.aabbMin;
  int axis = 0;
  if (extent.y > extent.x) axis = 1;
  if (extent.z > extent[axis]) axis = 2;
  float splitPos = node.aabbMin[axis] + extent[axis] * 0.5f;

  int i = node.firstObject;
  int j = node.firstObject + node.numObjects - 1;
  while (i <= j) {
    GpuObject &object = _gpuObjects[i];
    if (object.type == 0) {
      if (object.data[axis] < splitPos) {
        i++;
      } else {
        std::swap(_gpuObjects.at(i), _gpuObjects.at(j--));
      }
    } else if (object.type == 1) {
      glm::vec3 v0 = _vertices[object.data.x].position;
      glm::vec3 v1 = _vertices[object.data.y].position;
      glm::vec3 v2 = _vertices[object.data.z].position;
      glm::vec3 centroid = (v0 + v1 + v2) / 3.0f;
      if (centroid[axis] < splitPos) {
        i++;
      } else {
        std::swap(_gpuObjects.at(i), _gpuObjects.at(j--));
      }
    }
  }

  int leftCount = i - node.firstObject;
  if (leftCount == 0 || leftCount == node.numObjects) {
    return;
  }

  int leftIndex = _nodesUsed++;
  int rightIndex = _nodesUsed++;

  _nodes[leftIndex].firstObject = 0;
  _nodes[leftIndex].numObjects = leftCount;

  _nodes[rightIndex].firstObject = i;
  _nodes[rightIndex].numObjects = node.numObjects - leftCount;

  node.leftChild = leftIndex;
  node.numObjects = 0;

  updateNodeBounds(leftIndex);
  updateNodeBounds(rightIndex);
  subdivide(leftIndex);
  subdivide(rightIndex);
}
