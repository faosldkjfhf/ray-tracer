#pragma once

#include "core/Material.hpp"
#include "core/Object.hpp"
#include "core/Sphere.hpp"
#include "rendering/Triangle.hpp"

#include <vector>
#include <iostream>

struct Vertex {
  alignas(16) glm::vec3 position;
};

struct Face {
  // unsigned int v0;
  // unsigned int v1;
  // unsigned int v2;
  // unsigned int materialIdx;
  glm::uvec4 indicesAndMaterialIdx;
};

struct Scene {
  std::vector<Sphere> spheres;
  std::vector<Triangle> triangles;
  std::vector<Object> objects;
  std::vector<Material> materials;

  void update() {
    for (auto &object : objects) {
      object.getTransform().computeModelMatrix();
    }
  }

  std::vector<Vertex> getVertices() const {
    std::vector<Vertex> vertices;

    for (const auto &object : objects) {
      const auto &mesh = object.getMesh();
      const auto &transform = object.getTransform();

      for (const auto &vertex : mesh.vertices) {
        glm::vec4 position =
            transform.getModelMatrix() * glm::vec4(vertex.position, 1.0f);
        vertices.push_back({position});
      }
    }

    return vertices;
  }

  std::vector<Face> getFaces() const {
    std::vector<Face> faces;

    unsigned int offset = 0;

    for (const auto &object : objects) {
      for (int i = 0; i < object.getMesh().indices.size(); i += 3) {
        glm::uvec4 indicesAndMaterialIdx = {
            object.getMesh().indices[i + 0] + offset,
            object.getMesh().indices[i + 1] + offset,
            object.getMesh().indices[i + 2] + offset, 0};
        faces.push_back({indicesAndMaterialIdx});
      }
      offset += object.getMesh().vertices.size();
    }

    return faces;
  }
};
