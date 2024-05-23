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
      std::cout << "Added material to scene, now has " << materials.size()
                << " materials\n";
    }
  }

  // add the faces into the gpuObjects vector
  std::vector<GpuObject> gpuObjects;
  for (auto &face : getFaces()) {
    gpuObjects.push_back({{face.v0, face.v1, face.v2, 0.0f},
                          GpuObjectType::Face,
                          face.materialIdx});
  }

  // add the spheres into the gpuObjects vector
  for (auto &sphere : spheres) {
    // Find the sphere's material index
    const auto &material = sphere.material;
    auto materialIt = std::find(materials.begin(), materials.end(), material);
    unsigned int materialIdx = 0;
    if (materialIt == materials.end()) {
      materials.push_back(material);
      materialIdx = materials.size() - 1;
    } else {
      materialIdx = std::distance(materials.begin(), materialIt);
    }
    gpuObjects.push_back(
        {{sphere.center.x, sphere.center.y, sphere.center.z, sphere.radius},
         GpuObjectType::Sphere,
         materialIdx});
  }

  bvh.buildBVH(gpuObjects, getVertices());
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
    uint32_t materialIdx = std::distance(materials.begin(), materialIt);

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
