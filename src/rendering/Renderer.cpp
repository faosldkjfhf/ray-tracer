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
  _texture.bind(_shader, 0);
}

void Renderer::render(const Scene &scene) const {
  // Enable depth test and face culling (to fix shadow peter panning)
  /*
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  */

  _computeShader.use();

  // Pass in scene data as uniforms
  _computeShader.setVec3("u_CameraPosition", _camera.getPosition());
  glCall(glDispatchCompute((GLuint)_window->getWidth() / 10,
                           (GLuint)_window->getHeight() / 10, 1));
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);

  // Reset viewport
  // glViewport(0, 0, _window->getWidth(), _window->getHeight());

  // Render the screen quad
  _shader.use();
  // _screenQuadLayout.bind();
  // _texture.bind(_shader, 0);

  glDrawElements(GL_TRIANGLES, _screenQuad.indices.size(), GL_UNSIGNED_INT,
                 nullptr);
}

void Renderer::flipPolygonMode() {
  _polygonMode = _polygonMode == GL_FILL ? GL_LINE : GL_FILL;
}
