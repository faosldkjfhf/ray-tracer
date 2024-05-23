#include "core/SDLGraphicsProgram.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "core/Error.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Window.hpp"

SDLGraphicsProgram::SDLGraphicsProgram(Window *window, Renderer *renderer)
    : _window(window), _renderer(renderer) {
  initCornellBox();
  initBuffers();
};

void SDLGraphicsProgram::input(float deltaTime) {
  int mouseY = _window->getHeight() / 2;
  int mouseX = _window->getWidth() / 2;
  float moveSpeed = 50.0f * deltaTime;
  Camera &camera = _renderer->getCamera();

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
      camera.mouseLook(mouseX, mouseY);
      _renderer->setFrameCount(0);
    default:
      break;
    }
  }

  // Retrieve keyboard state
  const Uint8 *state = SDL_GetKeyboardState(NULL);

  // Camera
  // Update our position of the camera
  if (state[SDL_SCANCODE_W]) {
    camera.moveForward(moveSpeed);
    _renderer->setFrameCount(0);
  }
  if (state[SDL_SCANCODE_S]) {
    camera.moveBackward(moveSpeed);
    _renderer->setFrameCount(0);
  }
  if (state[SDL_SCANCODE_A]) {
    camera.moveLeft(moveSpeed);
    _renderer->setFrameCount(0);
  }
  if (state[SDL_SCANCODE_D]) {
    camera.moveRight(moveSpeed);
    _renderer->setFrameCount(0);
  }
  if (state[SDL_SCANCODE_SPACE]) {
    camera.moveUp(moveSpeed);
    _renderer->setFrameCount(0);
  }
  if (state[SDL_SCANCODE_Q]) {
    camera.moveDown(moveSpeed);
    _renderer->setFrameCount(0);
  }

  // Toggle polygon mode
  if (state[SDL_SCANCODE_TAB]) {
    SDL_Delay(200);
    _renderer->flipPolygonMode();
  }

  _lastTime = SDL_GetTicks();
}

void SDLGraphicsProgram::update(float deltaTime) {}

void SDLGraphicsProgram::render() const {
  _renderer->render(_scene);
  _renderer->setFrameCount(_renderer->getFrameCount() + 1);
}

void SDLGraphicsProgram::run() {
  /*
  SDL_WarpMouseInWindow(_window->getWindow(), _window->getWidth() / 2,
                        _window->getHeight() / 2);*/
  getOpenGLVersionInfo();

  _gpuObjectBuffer.bind();
  _vertexBuffer.bind();
  _materialBuffer.bind();
  _bvhBuffer.bind();

  _lastTime = SDL_GetTicks();
  int frameCount = 0;
  while (!_quit) {
    Uint32 currentTime = SDL_GetTicks();
    Uint32 delta = std::max((Uint32)1, currentTime - _lastTime);
    _lastTime = currentTime;

    glCheckError("run", 126);

    float deltaTime = delta / 1000.0f;
    input(deltaTime);
    update(deltaTime);
    render();

    glCheckError("run", 132);

    if (frameCount > 100) {
      std::cout << "FPS: " << 1000.0f / delta << std::endl;
      frameCount = 0;
    } else {
      frameCount++;
    }

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

void SDLGraphicsProgram::initCornellBox() {
  Material white = Material::lambertian(glm::vec3(0.73f));
  Material red = Material::lambertian(glm::vec3(0.65f, 0.05f, 0.05f));
  Material green = Material::lambertian(glm::vec3(0.12f, 0.45f, 0.15f));

  // Floor
  Mesh floor;
  floor.vertices = {{{0.0f, 0.0f, 0.0f}},
                    {{555.0f, 0.0f, 0.0f}},
                    {{555.0f, 0.0f, -555.0f}},
                    {{0.0f, 0.0f, -555.0f}}};
  floor.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({floor, white});

  // Ceiling
  Mesh ceiling;
  ceiling.vertices = {{{0.0f, 555.0f, 0.0f}},
                      {{555.0f, 555.0f, 0.0f}},
                      {{555.0f, 555.0f, -555.0f}},
                      {{0.0f, 555.0f, -555.0f}}};
  ceiling.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({ceiling, white});

  // Back wall
  Mesh backWall;
  backWall.vertices = {{{0.0f, 0.0f, -555.0f}},
                       {{555.0f, 0.0f, -555.0f}},
                       {{555.0f, 555.0f, -555.0f}},
                       {{0.0f, 555.0f, -555.0f}}};
  backWall.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({backWall, white});

  // Left wall
  Mesh leftWall;
  leftWall.vertices = {{{0.0f, 0.0f, 0.0f}},
                       {{0.0f, 0.0f, -555.0f}},
                       {{0.0f, 555.0f, -555.0f}},
                       {{0.0f, 555.0f, 0.0f}}};
  leftWall.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({leftWall, red});

  // Right wall
  Mesh rightWall;
  rightWall.vertices = {{{555.0f, 0.0f, 0.0f}},
                        {{555.0f, 0.0f, -555.0f}},
                        {{555.0f, 555.0f, -555.0f}},
                        {{555.0f, 555.0f, 0.0f}}};
  rightWall.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({rightWall, green});

  // Light
  Mesh light;
  light.vertices = {
      {{343.0f, 554.0f, -332.0f}},
      {{343.0f, 554.0f, -423.0f}},
      {{213.0f, 554.0f, -423.0f}},
      {{213.0f, 554.0f, -332.0f}},
  };
  light.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({light, Material::light()});

  // Short block
  // Mesh shortBlock;
  // shortBlock.vertices = {
  //     // Front
  //   {{130.0f, 0.0f, }}
  // };

  // Add some objects
  _scene.spheres.push_back(
      {{190.0f, 90.0f, -190.0f}, 90.0f, Material::dielectric(1.5f)});
  _scene.spheres.push_back({{400.0f, 80.0f, -400.0f},
                            80.0f,
                            Material::metal(glm::vec3(1.0f), 0.0f)});

  Object bunny("res/models/bunny/bunny_centered_reduced_fixed.obj");
  bunny.transform.setPosition(300.0f, 300.0f, -300.0f);
  bunny.transform.setScale(100.0f, 100.0f, 100.0f);
  _scene.objects.push_back(bunny);

  // Update the camera
  _renderer->getCamera().setPosition(278.0f, 278.0f, 250.0f);
}

void SDLGraphicsProgram::initBuffers() {
  _scene.update();

  _gpuObjectBuffer.createStorageBuffer(_scene.bvh.getGpuObjects(),
                                       GL_STATIC_DRAW, 1);
  _vertexBuffer.createStorageBuffer(_scene.getVertices(), GL_STATIC_DRAW, 2);
  _materialBuffer.createStorageBuffer(_scene.materials, GL_STATIC_DRAW, 3);
  _bvhBuffer.createStorageBuffer(_scene.bvh.getBVHNodes(), GL_STATIC_DRAW, 4);
}
