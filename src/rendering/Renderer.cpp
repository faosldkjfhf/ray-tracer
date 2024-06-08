#include "rendering/Renderer.hpp"

#include "rendering/Window.hpp"

#include "glad/glad.h"

#include "imgui.h"
#include "imgui_impl_opengl3.h"

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
  _computeShader.bindTextures(scene.textures, 1);

  glDispatchCompute((GLuint)_window->getWidth() / 32,
                    (GLuint)_window->getHeight() / 32, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  // Render the screen quad
  _shader.use();
  glDrawElements(GL_TRIANGLES, _screenQuad.indices.size(), GL_UNSIGNED_INT,
                 nullptr);

  if (_debug && _debugFBO) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, _debugFBO);
    glBlitFramebuffer(0, 0, _window->getWidth(), _window->getHeight(), 0, 0,
                      _window->getWidth(), _window->getHeight(),
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  }

  // Render ImGui
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::createDebugFBO(unsigned int textureID) {
  glGenFramebuffers(1, &_debugFBO);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _debugFBO);
  glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, textureID, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
