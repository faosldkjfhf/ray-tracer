#pragma once

#include <string>

class Shader;

class Texture {
public:
  enum TextureType { DIFFUSE, SPECULAR, NORMAL };

  Texture() = default;
  Texture(const std::string &path, TextureType type);
  Texture(int width, int height);
  ~Texture();

  void loadFromFile();
  void bind(const Shader &shader, unsigned int slot = 0) const;
  void unbind() const;

private:
  unsigned int _id;
  TextureType _type;
  std::string _path;
};
