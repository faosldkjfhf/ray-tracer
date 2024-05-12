#include "rendering/BVH.hpp"

#include <algorithm>

void BVH::buildBVH(const std::vector<Face> &faces,
                   const std::vector<Vertex> &vertices) {
    _faces = faces;
    _vertices = vertices;

    _size = faces.size() * 2 - 1;
    _nodes.resize(_size);

    BVHNode &root = _nodes[_rootIndex];
    root.firstObject = 0;
    root.leftChild = 0;
    root.numObjects = _size;

    updateNodeBounds(_rootIndex);
    subdivide(_rootIndex);
}

void BVH::updateNodeBounds(unsigned int nodeIndex) {
    BVHNode &node = _nodes[nodeIndex];
    for (int i = node.firstObject; i < node.firstObject + node.numObjects;
         i++) {
        glm::vec3 v0 = _vertices[_faces[i].v0].position;
        glm::vec3 v1 = _vertices[_faces[i].v1].position;
        glm::vec3 v2 = _vertices[_faces[i].v2].position;
        node.aabbMin.x = fminf(node.aabbMin.x, fminf(fminf(v0.x, v1.x), v2.x));
        node.aabbMin.z = fminf(node.aabbMin.z, fminf(fminf(v0.z, v1.z), v2.z));
        node.aabbMin.y = fminf(node.aabbMin.y, fminf(fminf(v0.y, v1.y), v2.y));

        node.aabbMax.x = fmaxf(node.aabbMax.x, fmaxf(fmaxf(v0.x, v1.x), v2.x));
        node.aabbMax.y = fmaxf(node.aabbMax.y, fmaxf(fmaxf(v0.y, v1.y), v2.y));
        node.aabbMax.z = fmaxf(node.aabbMax.z, fmaxf(fmaxf(v0.z, v1.z), v2.z));
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
        glm::vec3 v0 = _vertices[_faces[i].v0].position;
        glm::vec3 v1 = _vertices[_faces[i].v1].position;
        glm::vec3 v2 = _vertices[_faces[i].v2].position;
        glm::vec3 centroid = (v0 + v1 + v2) / 3.0f;
        if (centroid[axis] < splitPos) {
            i++;
        } else {
            std::swap(_faces.at(i), _faces.at(j--));
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