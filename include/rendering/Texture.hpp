#pragma once

#include <string>

class Shader;

class Texture {
public:
  enum TextureType { DIFFUSE, SPECULAR, NORMAL };

  TextureType type;

  Texture() = default;
  Texture(const std::string &path, TextureType type);
  Texture(int width, int height);
  ~Texture();

  bool operator==(const Texture &other) const {
    return _id == other._id && type == other.type && _path == other._path;
  }

  void loadFromFile();
  void bind(unsigned int slot = 0) const;
  void unbind() const;

private:
  unsigned int _id;
  std::string _path;
};
