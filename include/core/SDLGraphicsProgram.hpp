#pragma once

#include <SDL2/SDL.h>

class Window;
class Renderer;

class SDLGraphicsProgram {
public:
  SDLGraphicsProgram(Window *window, Renderer *renderer);

  void run();

  void getOpenGLVersionInfo();

  // initializing a test scene
  void init();

private:
  bool _quit = false;

  // Stored to prevent a large delta time after delays
  Uint32 _lastTime;

  Window *_window;
  Renderer *_renderer;

  void input(float deltaTime);
  void update(float deltaTime);
  void render() const;
};
