#pragma once

#include <string>

class Shader;

class Texture {
public:
  enum TextureType { DIFFUSE, SPECULAR, NORMAL };

  unsigned int id;
  TextureType type;

  Texture() = default;
  Texture(const std::string &path, TextureType type);
  Texture(int width, int height);
  ~Texture();

  bool operator==(const Texture &other) const {
    return id == other.id && type == other.type && _path == other._path;
  }

  void loadFromFile();
  void bind(unsigned int slot = 0) const;
  void unbind() const;

private:
  std::string _path;
};
