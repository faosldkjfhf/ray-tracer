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

  Camera &getCamera() { return _camera; }

  void incrementFrameCount() { _frameCount++; }
  void resetFrameCount() { _frameCount = 0; }

  void flipDebug() { _debug = !_debug; }

  void createDebugFBO(unsigned int textureID);

private:
  Camera _camera;
  uint _frameCount = 0;

  Shader _shader;
  Shader _computeShader;

  Mesh _screenQuad;
  VertexBufferLayout _screenQuadLayout;
  Texture _texture;

  // Debugging
  bool _debug = false;
  GLuint _debugFBO = 0;

  const Window *_window;
};
