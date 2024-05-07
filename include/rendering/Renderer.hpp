#pragma once

#include "core/Scene.hpp"
#include "core/Camera.hpp"

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

  void flipPolygonMode();

  void setupTexture();

  Camera &getCamera() { return _camera; }
  // Light &getLight() { return _light; }

private:
  Camera _camera;

  Shader _shader;
  Shader _computeShader;

  Mesh _screenQuad;
  VertexBufferLayout _screenQuadLayout;
  Texture _texture;

  // Shader _lightShader;
  // Shader _depthShader;
  // DepthMap _depthMap;

  // Shader _debugDepthShader;

  // std::vector<Light> _lights;
  // unsigned int _activeLight = 0;
  // Light _light;

  const Window *_window;
  GLenum _polygonMode = GL_FILL;
};
