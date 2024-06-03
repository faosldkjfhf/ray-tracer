#include "rendering/Renderer.hpp"

#include "core/Error.hpp"
#include "glad/glad.h"
#include "rendering/Window.hpp"

Renderer::Renderer(const Window &window)
    : _camera(window.getWidth(), window.getHeight()),
      _shader("shaders/vert.glsl", "shaders/frag.glsl"),
      _computeShader("shaders/compute.glsl"),
      _texture(window.getWidth(), window.getHeight()), _window(&window) {
  std::vector<MeshVertex> vertices = {
      {{-1, -1, 0}, {0, 0}, {0, 0, 1}},
      {{1, -1, 0}, {1, 0}, {0, 0, 1}},
      {{1, 1, 0}, {1, 1}, {0, 0, 1}},
      {{-1, 1, 0}, {0, 1}, {0, 0, 1}},
  };

  std::vector<unsigned int> indices = {0, 1, 2, 0, 2, 3};

  _screenQuad = {vertices, indices};
  _screenQuadLayout.createBufferLayout(_screenQuad.vertices,
                                       _screenQuad.indices);

  _shader.use();
  _screenQuadLayout.bind();
  _texture.bind(0);
}

void Renderer::render(const Scene &scene) const {
  _texture.bind(0);

  _computeShader.use();
  // Pass in scene data as uniforms
  _computeShader.setVec3("u_CameraPosition", _camera.getPosition());
  _computeShader.setVec3("u_CameraDirection", _camera.getViewDirection());
  _computeShader.setVec3("u_CameraUp", _camera.getUpVector());
  _computeShader.setUInt("u_FrameCount", _frameCount);
  _computeShader.bindTextures(scene.textures);

  glCall(glDispatchCompute((GLuint)_window->getWidth() / 32,
                           (GLuint)_window->getHeight() / 32, 1));
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);

  // Render the screen quad
  _shader.use();
  glDrawElements(GL_TRIANGLES, _screenQuad.indices.size(), GL_UNSIGNED_INT,
                 nullptr);
}

void Renderer::flipPolygonMode() {
  _polygonMode = _polygonMode == GL_FILL ? GL_LINE : GL_FILL;
}
