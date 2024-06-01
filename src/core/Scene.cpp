#include "core/Scene.hpp"

#include <algorithm>
#include <iostream>

void Scene::update() {
  gpuTextures.resize(objects.size());

  for (int i = 0; i < objects.size(); i++) {
    auto &object = objects[i];

    object.transform.computeModelMatrix();

    // Find the objects material index, add to the materials vector if it
    // doesn't exist
    const auto &material = object.material;
    auto materialIt = std::find(materials.begin(), materials.end(), material);
    if (materialIt == materials.end()) {
      materials.push_back(material);
    }

    // Add all the object's textures
    for (auto &texture : object.textures) {
      uint32_t textureIdx = 0;
      auto textureIt = std::find(textures.begin(), textures.end(), texture);
      if (textureIt == textures.end()) {
        textureIdx = textures.size();
        textures.push_back(texture);
      } else {
        textureIdx = std::distance(textures.begin(), textureIt);
      }

      if (texture.type == Texture::TextureType::DIFFUSE) {
        gpuTextures[i].diffuseTextureIdx = textureIdx;
      } else if (texture.type == Texture::TextureType::NORMAL) {
        gpuTextures[i].normalTextureIdx = textureIdx;
      }
      // std::cout << "Texture index: " << textureIdx << " for object " << i
      //           << std::endl;
    }
  }

  // Add the faces into the gpuObjects vector
  std::vector<GpuObject> gpuObjects;
  for (auto &face : getFaces()) {
    gpuObjects.push_back({{face.v0, face.v1, face.v2, 0.0f},
                          GpuObjectType::Face,
                          face.materialIdx,
                          face.gpuTextureIdx});
  }

  // Add the spheres into the gpuObjects vector
  for (auto &sphere : spheres) {
    // Find the sphere's material index, add to the materials vector if it
    // doesn't exist
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

void Scene::bindTextures(const Shader &shader) const {
  for (size_t i = 0; i < textures.size(); i++) {
    textures[i].bind(i + 1);
    // shader.setInt("u_Textures[" + std::to_string(i) + "]", i + 1);
  }
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

    const auto &indices = object.mesh.indices;
    for (size_t i = 0; i < indices.size(); i += 3) {
      faces.push_back({indices[i] + offset, indices[i + 1] + offset,
                       indices[i + 2] + offset, materialIdx, (uint32_t)i});
    }

    offset += object.mesh.vertices.size();
  }

  std::cout << "Number of faces: " << faces.size() << std::endl;

  return faces;
}
