#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class Light;

class Shader {
public:
  unsigned int id;

  Shader() = default;
  Shader(const std::string &vertexPath, const std::string &fragmentPath);
  ~Shader() { glDeleteProgram(id); }

  void load(const std::string &vertexPath, const std::string &fragmentPath);

  void use() const;

  void setBool(const std::string &name, bool value) const;
  void setInt(const std::string &name, int value) const;
  void setFloat(const std::string &name, float value) const;
  void setVec2(const std::string &name, const glm::vec2 &value) const;
  void setVec2(const std::string &name, float x, float y) const;
  void setVec3(const std::string &name, const glm::vec3 &value) const;
  void setVec3(const std::string &name, float x, float y, float z) const;
  void setVec4(const std::string &name, const glm::vec4 &value) const;
  void setVec4(const std::string &name, float x, float y, float z,
               float w) const;
  void setMat2(const std::string &name, const glm::mat2 &mat) const;
  void setMat3(const std::string &name, const glm::mat3 &mat) const;
  void setMat4(const std::string &name, const glm::mat4 &mat) const;
  // void setLight(const std::string &name, const Light &light) const;
};
