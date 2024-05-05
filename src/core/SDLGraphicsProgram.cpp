#include "core/SDLGraphicsProgram.hpp"

#include "core/Error.hpp"

#include "rendering/MeshVertex.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Window.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <iostream>

SDLGraphicsProgram::SDLGraphicsProgram(Window *window, Renderer *renderer)
    : _window(window), _renderer(renderer){};

void SDLGraphicsProgram::input(float deltaTime) {
  int mouseY = _window->getHeight() / 2;
  int mouseX = _window->getWidth() / 2;
  float moveSpeed = 20.0f * deltaTime;
  // Camera &camera = _renderer->getCamera();

  // Event handler that handles various events in SDL
  // that are related to input and output
  SDL_Event e;
  // Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    switch (e.type) {
    case SDL_EVENT_QUIT:
      std::cout << "Goodbye!" << std::endl;
      _quit = true;
      break;
    case SDL_EVENT_KEY_DOWN:
      if (e.key.keysym.sym == SDLK_ESCAPE) {
        std::cout << "ESC: Goodbye!" << std::endl;
        _quit = true;
      }
      break;
    case SDL_EVENT_MOUSE_MOTION:
      // Capture the change in the mouse position
      mouseX = e.motion.x;
      mouseY = e.motion.y;
      // camera.mouseLook(mouseX, mouseY);
    default:
      break;
    }
  }

  // Retrieve keyboard state
  const Uint8 *state = SDL_GetKeyboardState(NULL);

  /*
  // Camera
  // Update our position of the camera
  if (state[SDL_SCANCODE_W]) {
    camera.moveForward(moveSpeed);
  }
  if (state[SDL_SCANCODE_S]) {
    camera.moveBackward(moveSpeed);
  }
  if (state[SDL_SCANCODE_A]) {
    camera.moveLeft(moveSpeed);
  }
  if (state[SDL_SCANCODE_D]) {
    camera.moveRight(moveSpeed);
  }
  if (state[SDL_SCANCODE_SPACE]) {
    camera.moveUp(moveSpeed);
  }
  if (state[SDL_SCANCODE_Q]) {
    camera.moveDown(moveSpeed);
  }
  */

  // Toggle polygon mode
  if (state[SDL_SCANCODE_TAB]) {
    SDL_Delay(200);
    _renderer->flipPolygonMode();
  }

  _lastTime = SDL_GetTicks();
}

void SDLGraphicsProgram::update(float deltaTime) {}

void SDLGraphicsProgram::render() const { _renderer->render(); }

void SDLGraphicsProgram::run() {
  /*
  SDL_WarpMouseInWindow(_window->getWindow(), _window->getWidth() / 2,
                        _window->getHeight() / 2);*/
  // Disable for grabbing
  // SDL_SetRelativeMouseMode(SDL_TRUE);

  _lastTime = SDL_GetTicks();

  getOpenGLVersionInfo();

  while (!_quit) {
    Uint32 currentTime = SDL_GetTicks();
    Uint32 delta = std::max((unsigned int)1, currentTime - _lastTime);
    _lastTime = currentTime;

    glCheckError("run", 126);

    float deltaTime = delta / 1000.0f;
    input(deltaTime);
    update(deltaTime);
    render();

    glCheckError("run", 132);

    // if (delta < 16) {
    //   std::cout << "Delaying: " << 16 - delta << "ms" << std::endl;
    //   SDL_Delay(16 - delta);
    // }
    // std::cout << "delta: " << delta << "ms\n";

    _window->swapBuffers();
  }
}

void SDLGraphicsProgram::getOpenGLVersionInfo() {
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
  std::cout << "Version: " << glGetString(GL_VERSION) << "\n";
  std::cout << "Shading language: " << glGetString(GL_SHADING_LANGUAGE_VERSION)
            << "\n";
}

void SDLGraphicsProgram::init() {}
