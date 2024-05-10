#pragma once

#include "core/Camera.hpp"
#include "core/Scene.hpp"

#include "rendering/Mesh.hpp"
#include "rendering/Texture.hpp"
#include "rendering/VertexBufferLayout.hpp"

#include "Shader.hpp"

class Window;
class Entity;

class Renderer {
public:
  Renderer(const Window &window);

  void render(const Scene &scene) const;

  // void updateScene(const Scene &scene);

  void flipPolygonMode();

  Camera &getCamera() { return _camera; }

private:
  Camera _camera;

  Shader _shader;
  Shader _computeShader;

  Mesh _screenQuad;
  VertexBufferLayout _screenQuadLayout;
  Texture _texture;

  const Window *_window;
  GLenum _polygonMode = GL_FILL;
};
