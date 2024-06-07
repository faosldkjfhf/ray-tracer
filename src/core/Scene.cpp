#include "core/Scene.hpp"

#include <algorithm>
#include <iostream>

#include <SDL3/SDL.h>

void Scene::update() {
  for (int i = 0; i < objects.size(); i++) {
    auto &object = objects[i];

    object.transform.computeModelMatrix();

    // Add the object's material to the materials vector if it doesn't exist
    const auto &material = object.material;
    auto materialIt = std::find(materials.begin(), materials.end(), material);
    if (materialIt == materials.end()) {
      materials.push_back(material);
    }

    // Add the object's textures to the textures vector if they don't exist
    for (const auto &texture : object.textures) {
      auto textureIt = std::find(textures.begin(), textures.end(), texture);
      if (textureIt == textures.end()) {
        textures.push_back(texture);
      }
    }
  }

  // Add the faces into the gpuObjects vector
  std::vector<GpuObject> gpuObjects;
  for (auto &face : getFaces()) {
    gpuObjects.push_back({{face.v0, face.v1, face.v2, 0.0f},
                          ObjectType::Face,
                          face.materialIdx,
                          face.textureIndices});
  }

  // Add the spheres into the gpuObjects vector
  for (auto &sphere : spheres) {
    // Find the sphere's material index, add to the materials vector if it
    // doesn't exist
    const auto &material = sphere.material;
    unsigned int materialIdx = 0;
    auto materialIt = std::find(materials.begin(), materials.end(), material);
    if (materialIt == materials.end()) {
      materialIdx = materials.size();
      materials.push_back(material);
    } else {
      materialIdx = std::distance(materials.begin(), materialIt);
    }

    gpuObjects.push_back(
        {{sphere.center.x, sphere.center.y, sphere.center.z, sphere.radius},
         ObjectType::Sphere,
         materialIdx,
         {-1, -1}});
  }

  // Time how long it takes to build the BVH
  auto start = SDL_GetTicks();
  bvh.buildBVH(gpuObjects, getVertices());
  auto end = SDL_GetTicks();
  std::cout << "BVH build time: " << end - start << "ms" << std::endl;
}

std::vector<Vertex> Scene::getVertices() const {
  std::vector<Vertex> vertices;

  for (const auto &object : objects) {
    const auto &modelMatrix = object.transform.getModelMatrix();

    for (const auto &vertex : object.mesh.vertices) {
      glm::vec4 position = modelMatrix * glm::vec4(vertex.position, 1.0f);
      vertices.push_back({position, vertex.uv});
    }
  }

  std::cout << "Number of vertices: " << vertices.size() << std::endl;

  return vertices;
}

std::vector<Face> Scene::getFaces() const {
  std::vector<Face> faces;

  unsigned int offset = 0;

  for (size_t i = 0; i < objects.size(); i++) {
    const auto &object = objects[i];

    // Find the objects material index
    const auto &material = object.material;
    auto materialIt = std::find(materials.begin(), materials.end(), material);
    if (materialIt == materials.end()) {
      std::cerr << "Material not found in scene" << std::endl;
      continue;
    }
    uint32_t materialIdx = std::distance(materials.begin(), materialIt);

    // Find the objects texture index
    glm::ivec2 textureIndices = {-1, -1};
    for (auto &texture : object.textures) {
      auto textureIt = std::find(textures.begin(), textures.end(), texture);
      if (textureIt == textures.end()) {
        std::cerr << "Texture not found in scene" << std::endl;
        continue;
      }
      uint32_t textureIdx = std::distance(textures.begin(), textureIt);
      if (texture.type == Texture::TextureType::DIFFUSE) {
        textureIndices.x = textureIdx;
      } else if (texture.type == Texture::TextureType::NORMAL) {
        textureIndices.y = textureIdx;
      }
    }

    const auto &indices = object.mesh.indices;
    for (size_t i = 0; i < indices.size(); i += 3) {
      faces.push_back({indices[i] + offset, indices[i + 1] + offset,
                       indices[i + 2] + offset, materialIdx, textureIndices});
    }

    offset += object.mesh.vertices.size();
  }

  std::cout << "Number of faces: " << faces.size() << std::endl;

  return faces;
}
