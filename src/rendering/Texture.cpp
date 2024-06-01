#include "rendering/Texture.hpp"

#include "rendering/PPM.hpp"
#include "rendering/Shader.hpp"

#include <glad/glad.h>

Texture::Texture(const std::string &path, TextureType type)
    : _path(path), type(type) {}

Texture::Texture(int width, int height) {
  glGenTextures(1, &_id);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _id);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
               GL_FLOAT, NULL);
  glBindImageTexture(0, _id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

Texture::~Texture() { glDeleteTextures(1, &_id); }

void Texture::loadFromFile() {
  glGenTextures(1, &_id);
  glBindTexture(GL_TEXTURE_2D, _id);

  // set the texture wrapping/filtering options (on the currently bound
  // texture object)
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // load and generate the texture
  PPM image(_path);
  image.flipVertical();
  image.flipHorizontal();
  int width = image.getWidth();
  int height = image.getHeight();
  int numChannels = 3;
  uint8_t *data = image.pixelDataPtr();

  GLenum format;
  if (numChannels == 1) {
    format = GL_RED;
  } else if (numChannels == 3) {
    format = GL_RGB;
  } else if (numChannels == 4) {
    format = GL_RGBA;
  }
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(unsigned int slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  // std::string name = "u_Material.";
  // if (_type == TextureType::DIFFUSE) {
  //   name += "diffuse";
  // } else if (_type == TextureType::SPECULAR) {
  //   name += "specular";
  // } else if (_type == TextureType::NORMAL) {
  //   name += "normal";
  // }
  // std::string name = "u_Texture";
  // shader.setInt(name, slot);
  glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture::unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }
