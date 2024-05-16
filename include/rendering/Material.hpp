#pragma once
#include <glm/glm.hpp>

enum class MaterialType { LAMBERTIAN, METAL, GLASS, LIGHT };

struct Material {
  alignas(16) glm::vec3 color;
  MaterialType type;
  alignas(4) float typeData{0.0f}; // Metal: fuzziness, Glass: refraction index

  bool operator==(const Material &other) const {
    return color == other.color && type == other.type && typeData == other.typeData;
  }

  static Material red() {
    return Material{glm::vec3(1.0f, 0.0f, 0.0f), MaterialType::LAMBERTIAN};
  }

  static Material green() {
    return Material{glm::vec3(0.0f, 1.0f, 0.0f), MaterialType::LAMBERTIAN};
  }

  static Material blue() {
    return Material{glm::vec3(0.0f, 0.0f, 1.0f), MaterialType::LAMBERTIAN};
  }
};
