#pragma once

#include <SDL3/SDL.h>

#include "core/Scene.hpp"
#include "core/StorageBuffer.hpp"

class Window;
class Renderer;

class SDLGraphicsProgram {
public:
  SDLGraphicsProgram(Window *window, Renderer *renderer);

  void run();

private:
  bool _quit = false;

  // Stored to prevent a large delta time after delays
  Uint32 _lastTime;

  Window *_window;
  Renderer *_renderer;

  Scene _scene;

  StorageBuffer _vertexBuffer;
  StorageBuffer _gpuObjectBuffer;
  StorageBuffer _bvhBuffer;
  StorageBuffer _materialBuffer;

  void getOpenGLVersionInfo();

  void input(float deltaTime);
  void update(float deltaTime);
  void render() const;

  void initCornellBox();
  void initBuffers();
};
