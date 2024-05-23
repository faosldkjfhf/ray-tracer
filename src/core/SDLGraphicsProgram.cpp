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
  // init();
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

  _spheresBuffer.bind();
  _vertexBuffer.bind();
  _faceBuffer.bind();
  _materialBuffer.bind();

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

// void SDLGraphicsProgram::init() {
//   // Create some spheres in the scene
//
//   // Floor
//   _scene.spheres.push_back({{0.0f, -100.5f, -1.0f}, 100.0f, 1});
//
//   _scene.spheres.push_back({{-1.0f, 0.0f, -1.0f}, 0.5f, 3});
//
//   // Metal sphere
//   _scene.spheres.push_back({{0.0f, 0.0f, -1.0f}, 0.5f, 4});
//
//   // Glass spheres
//   _scene.spheres.push_back({{1.0f, 0.0f, -1.0f}, 0.5f, 5});
//   _scene.spheres.push_back({{1.0f, 0.0f, -1.0f}, 0.4f, 6});
//
//   // 0. red, 1. green, 2. blue, 3. white light 4. metal
//   _scene.materials.push_back({{1.0f, 0.0f, 0.0f}, MaterialType::LAMBERTIAN});
//   _scene.materials.push_back({{0.0f, 1.0f, 0.0f}, MaterialType::LAMBERTIAN});
//   _scene.materials.push_back({{0.0f, 0.0f, 1.0f}, MaterialType::LAMBERTIAN});
//   _scene.materials.push_back({glm::vec3(1.0f), MaterialType::LIGHT});
//   _scene.materials.push_back({{1.0f, 1.0f, 1.0f}, MaterialType::METAL,
//   0.1f}); _scene.materials.push_back(
//       {{1.0f, 1.0f, 1.0f}, MaterialType::DIELECTRIC, 1.5f});
//   _scene.materials.push_back(
//       {{1.0f, 1.0f, 1.0f}, MaterialType::DIELECTRIC, 1.0f / 1.5f});
//
//   // Create some objects in the scene
//   Object cube("res/models/cube/cube.obj");
//   cube.transform.setPosition(1.0f, 1.0f, -3.0f);
//   _scene.objects.push_back(cube);
// }

void SDLGraphicsProgram::initCornellBox() {
  /* world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0),
    vec3(0,0,555), green)); world.add(make_shared<quad>(point3(0,0,0),
    vec3(0,555,0), vec3(0,0,555), red)); world.add(make_shared<quad>(point3(343,
    554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555),
    white)); world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0),
    vec3(0,0,-555), white)); world.add(make_shared<quad>(point3(0,0,555),
    vec3(555,0,0), vec3(0,555,0), white));
    */

  // Floor
  Mesh floor;
  floor.vertices = {{{0.0f, 0.0f, 0.0f}},
                    {{555.0f, 0.0f, 0.0f}},
                    {{555.0f, 0.0f, -555.0f}},
                    {{0.0f, 0.0f, -555.0f}}};
  floor.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({floor});

  // Ceiling
  Mesh ceiling;
  ceiling.vertices = {{{0.0f, 555.0f, 0.0f}},
                      {{555.0f, 555.0f, 0.0f}},
                      {{555.0f, 555.0f, -555.0f}},
                      {{0.0f, 555.0f, -555.0f}}};
  ceiling.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({ceiling});

  // Back wall
  Mesh backWall;
  backWall.vertices = {{{0.0f, 0.0f, -555.0f}},
                       {{555.0f, 0.0f, -555.0f}},
                       {{555.0f, 555.0f, -555.0f}},
                       {{0.0f, 555.0f, -555.0f}}};
  backWall.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({backWall});

  // Left wall
  Mesh leftWall;
  leftWall.vertices = {{{0.0f, 0.0f, 0.0f}},
                       {{0.0f, 0.0f, -555.0f}},
                       {{0.0f, 555.0f, -555.0f}},
                       {{0.0f, 555.0f, 0.0f}}};
  leftWall.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({leftWall, Material::red()});

  // Right wall
  Mesh rightWall;
  rightWall.vertices = {{{555.0f, 0.0f, 0.0f}},
                        {{555.0f, 0.0f, -555.0f}},
                        {{555.0f, 555.0f, -555.0f}},
                        {{555.0f, 555.0f, 0.0f}}};
  rightWall.indices = {0, 1, 2, 0, 2, 3};
  _scene.objects.push_back({rightWall, Material::green()});

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
