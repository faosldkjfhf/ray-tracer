#include "rendering/Renderer.hpp"

#include "rendering/Window.hpp"

Renderer::Renderer(const Window &window)
    : // _camera(window.getWidth(), window.getHeight()),
      _shader("res/shaders/shadow_vert.glsl", "res/shaders/shadow_frag.glsl"),
      _window(&window) {}

void Renderer::render() const {
  // Enable depth test and face culling (to fix shadow peter panning)
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Set the polygon fill mode
  glPolygonMode(GL_FRONT_AND_BACK, _polygonMode);

  // Initialize clear color
  // This is the background of the screen.
  glClearColor(0.1f, 4.f, 7.f, 1.f);

  // Clear color buffer and Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  // Reset viewport
  glViewport(0, 0, _window->getWidth(), _window->getHeight());
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void Renderer::flipPolygonMode() {
  _polygonMode = _polygonMode == GL_FILL ? GL_LINE : GL_FILL;
}
