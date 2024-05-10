#include "core/Scene.hpp"

#include <algorithm>
#include <iostream>

void Scene::update() {
  for (auto &object : objects) {
    object.transform.computeModelMatrix();

    // Find the objects material index
    const auto &material = object.material;
    auto materialIt = std::find(materials.begin(), materials.end(), material);
    if (materialIt == materials.end()) {
      materials.push_back(material);
    }
  }
}

std::vector<Vertex> Scene::getVertices() const {
  std::vector<Vertex> vertices;

  for (const auto &object : objects) {
    const auto &modelMatrix = object.transform.getModelMatrix();

    for (const auto &vertex : object.mesh.vertices) {
      glm::vec4 position = modelMatrix * glm::vec4(vertex.position, 1.0f);
      vertices.push_back({position});
    }
  }

  std::cout << "Number of vertices: " << vertices.size() << std::endl;

  return vertices;
}

std::vector<Face> Scene::getFaces() const {
  std::vector<Face> faces;

  unsigned int offset = 0;

  for (const auto &object : objects) {
    // Find the objects material index
    const auto &material = object.material;
    auto materialIt = std::find(materials.begin(), materials.end(), material);
    if (materialIt == materials.end()) {
      std::cerr << "Material not found in scene" << std::endl;
      continue;
    }
    unsigned int materialIdx = std::distance(materials.begin(), materialIt);

    const auto &indices = object.mesh.indices;

    for (size_t i = 0; i < indices.size(); i += 3) {
      faces.push_back({indices[i] + offset, indices[i + 1] + offset,
                       indices[i + 2] + offset, materialIdx});
    }

    offset += object.mesh.vertices.size();
  }

  std::cout << "Number of faces: " << faces.size() << std::endl;

  return faces;
}
