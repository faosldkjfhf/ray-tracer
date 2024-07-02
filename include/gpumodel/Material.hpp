#pragma once
#include <glm/glm.hpp>

enum class MaterialType { LAMBERTIAN, DIELECTRIC, LIGHT };

struct Material {
  alignas(16) glm::vec3 color;
  alignas(4) MaterialType type;
  alignas(4) float typeData{0.0f};
  // Lambert: smoothness; Dielectric: refraction index

  bool operator==(const Material &other) const {
    return color == other.color && type == other.type &&
           typeData == other.typeData;
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

  static Material white() {
    return Material{glm::vec3(1.0f), MaterialType::LAMBERTIAN};
  }

  static Material lambertian(const glm::vec3 &color = glm::vec3(1.0f)) {
    return Material{color, MaterialType::LAMBERTIAN};
  }

  static Material metal(const glm::vec3 &color = glm::vec3(1.0f),
                        float smoothness = 1.0f) {
    return Material{color, MaterialType::LAMBERTIAN, smoothness};
  }

  static Material metal(float smoothness) {
    return Material{glm::vec3(1.0f), MaterialType::LAMBERTIAN, smoothness};
  }

  static Material dielectric(const glm::vec3 &color = glm::vec3(1.0f),
                             float refractionIndex = 1.5f) {
    return Material{color, MaterialType::DIELECTRIC, refractionIndex};
  }

  static Material dielectric(float refractionIndex) {
    return Material{glm::vec3(1.0f), MaterialType::DIELECTRIC, refractionIndex};
  }

  static Material light(glm::vec3 color = glm::vec3(1.0f),
                        float intensity = 15.0f) {
    return Material{color * intensity, MaterialType::LIGHT};
  }

  static Material light(float intensity) {
    return Material{glm::vec3(intensity), MaterialType::LIGHT};
  }
};
