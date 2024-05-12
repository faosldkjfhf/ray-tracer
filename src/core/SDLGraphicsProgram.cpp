#include "core/SDLGraphicsProgram.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "core/Error.hpp"
#include "rendering/MeshVertex.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Window.hpp"

SDLGraphicsProgram::SDLGraphicsProgram(Window *window, Renderer *renderer)
    : _window(window), _renderer(renderer) {
  init();
};

void SDLGraphicsProgram::input(float deltaTime) {
  int mouseY = _window->getHeight() / 2;
  int mouseX = _window->getWidth() / 2;
  float moveSpeed = 5.0f * deltaTime;
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
      // camera.mouseLook(mouseX, mouseY);
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

  // Toggle polygon mode
  if (state[SDL_SCANCODE_TAB]) {
    SDL_Delay(200);
    _renderer->flipPolygonMode();
  }

  _lastTime = SDL_GetTicks();
}

void SDLGraphicsProgram::update(float deltaTime) {}

void SDLGraphicsProgram::render() const {
  // _spheresBuffer.bind();
  // _vertexBuffer.bind();
  // _faceBuffer.bind();
  // _materialBuffer.bind();
  _renderer->render(_scene);
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

void SDLGraphicsProgram::init() {
  // Create some spheres in the scene

  _scene.spheres.push_back({{-1.0f, 0.0f, -1.0f}, 0.5f, 3});
  // _scene.spheres.push_back({{0.0f, 0.0f, -1.0f}, 0.5, 3});
  // _scene.spheres.push_back({{1.0f, 0.0f, -1.0f}, 0.5f, 2});
  _scene.spheres.push_back({{0.0f, -100.5f, -1.0f}, 100.0f, 1});

  // 0. red, 1. green, 2. blue, 3. white light
  _scene.materials.push_back({{1.0f, 0.0f, 0.0f}, MaterialType::LAMBERTIAN});
  _scene.materials.push_back({{0.0f, 1.0f, 0.0f}, MaterialType::LAMBERTIAN});
  _scene.materials.push_back({{0.0f, 0.0f, 1.0f}, MaterialType::LAMBERTIAN});
  _scene.materials.push_back({glm::vec3(1.0f), MaterialType::LIGHT});

  // Create some objects in the scene
  Object cube("res/models/cube/cube.obj");
  cube.transform.setPosition(1.0f, 1.0f, -3.0f);
  _scene.objects.push_back(cube);
  // Mesh mesh;
  // mesh.vertices.push_back({{-0.5f, -0.5f, -2.0f}});
  // mesh.vertices.push_back({{0.5f, -0.5f, -1.0f}});
  // mesh.vertices.push_back({{0.0f, 0.5f, -2.0f}});
  // mesh.indices.push_back(0);
  // mesh.indices.push_back(1);
  // mesh.indices.push_back(2);
  // _scene.objects.push_back({mesh});
  // _scene.objects[0].material = _scene.materials[2];
  //
  // Mesh mesh2;
  // mesh2.vertices.push_back({{1.0f, -0.5f, -1.5f}});
  // mesh2.vertices.push_back({{0.5f, -0.5f, -2.0f}});
  // mesh2.vertices.push_back({{1.0f, 0.5f, -2.0f}});
  // mesh2.indices.push_back(0);
  // mesh2.indices.push_back(1);
  // mesh2.indices.push_back(2);
  // _scene.objects.push_back({mesh2});
  // _scene.objects[1].material = _scene.materials[0];

  _scene.update();

  // Create the storage buffers
  const auto &gpuObjects = _scene.bvh.getGpuObjects();
  const auto &vertices = _scene.getVertices();
  const auto &bvhNodes = _scene.bvh.getBVHNodes();

  _gpuObjectBuffer.createStorageBuffer(gpuObjects, GL_STATIC_DRAW, 1);
  _vertexBuffer.createStorageBuffer(vertices, GL_STATIC_DRAW, 2);
  _materialBuffer.createStorageBuffer(_scene.materials, GL_STATIC_DRAW, 3);
  _bvhBuffer.createStorageBuffer(bvhNodes, GL_STATIC_DRAW, 4);

  _gpuObjectBuffer.updateStorageBuffer(gpuObjects);
  _vertexBuffer.updateStorageBuffer(vertices);
  _materialBuffer.updateStorageBuffer(_scene.materials);
  _bvhBuffer.updateStorageBuffer(bvhNodes);
}
