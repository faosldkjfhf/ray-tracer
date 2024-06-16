#include "core/SDLGraphicsProgram.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "rendering/Renderer.hpp"
#include "rendering/Window.hpp"

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

SDLGraphicsProgram::SDLGraphicsProgram(Window *window, Renderer *renderer)
    : _window(window), _renderer(renderer) {
  initCornellBox();
  initObjects();

  _scene.update();
  initBuffers();

  if (_scene.textures.size() > 0)
    _renderer->createDebugFBO(_scene.textures[0].id);
};

void SDLGraphicsProgram::input(float deltaTime) {
  int mouseY = _window->getHeight() / 2;
  int mouseX = _window->getWidth() / 2;
  float moveSpeed = 500.0f * deltaTime;
  Camera &camera = _renderer->getCamera();
  bool shouldResetFrame = false;

  // Event handler that handles various events in SDL
  // that are related to input and output
  SDL_Event e;
  // Handle events on queue
  while (SDL_PollEvent(&e) != 0) {
    ImGui_ImplSDL3_ProcessEvent(&e);
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
      if (!_mouseCaptured) {
        break;
      }
      // Capture the change in the mouse position
      mouseX = e.motion.x;
      mouseY = e.motion.y;
      camera.mouseLook(mouseX, mouseY);
      shouldResetFrame = true;
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
    shouldResetFrame = true;
  }
  if (state[SDL_SCANCODE_S]) {
    camera.moveBackward(moveSpeed);
    shouldResetFrame = true;
  }
  if (state[SDL_SCANCODE_A]) {
    camera.moveLeft(moveSpeed);
    shouldResetFrame = true;
  }
  if (state[SDL_SCANCODE_D]) {
    camera.moveRight(moveSpeed);
    shouldResetFrame = true;
  }
  if (state[SDL_SCANCODE_SPACE]) {
    camera.moveUp(moveSpeed);
    shouldResetFrame = true;
  }
  if (state[SDL_SCANCODE_Q]) {
    camera.moveDown(moveSpeed);
    shouldResetFrame = true;
  }

  if (state[SDL_SCANCODE_TAB]) {
    _renderer->flipDebug();
    SDL_Delay(100);
  }

  if (state[SDL_SCANCODE_LCTRL]) {
    if (_mouseCaptured) {
      SDL_SetRelativeMouseMode(SDL_FALSE);
      _mouseCaptured = false;
    } else {
      SDL_SetRelativeMouseMode(SDL_TRUE);
      _mouseCaptured = true;
    }
    SDL_Delay(100);
  }

  if (shouldResetFrame) {
    _renderer->resetFrameCount();
  }

  _lastTime = SDL_GetTicks();
}

void SDLGraphicsProgram::update(float deltaTime) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
}

void SDLGraphicsProgram::render() const {
  _renderer->render(_scene);
  _renderer->incrementFrameCount();
}

void SDLGraphicsProgram::run() {
  getOpenGLVersionInfo();

  _vertexBuffer.bind();
  _gpuObjectBuffer.bind();
  _bvhBuffer.bind();
  _materialBuffer.bind();

  _lastTime = SDL_GetTicks();
  while (!_quit) {
    Uint32 currentTime = SDL_GetTicks();
    Uint32 delta = std::max((Uint32)1, currentTime - _lastTime);
    _lastTime = currentTime;

    float deltaTime = delta / 1000.0f;
    input(deltaTime);
    update(deltaTime);

    // render imgui window
    // TODO: Abstract out later?
    ImGui::Begin("Ray Tracer", NULL, ImGuiWindowFlags_None);
    ImGui::Text("FPS: %.2f", 1000.0f / delta);
    ImGui::End();

    render();

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

  Mesh quadMesh;
  quadMesh.vertices = {
      {{-0.5f, -0.5f, 0.0f}},
      {{0.5f, -0.5f, 0.0f}},
      {{0.5f, 0.5f, 0.0f}},
      {{-0.5f, 0.5f, 0.0f}},
  };
  quadMesh.indices = {0, 1, 2, 0, 2, 3};

  Object quadObj(quadMesh, white);

  // Floor
  quadObj.transform.setPosition(277.5f, 0.0f, -277.5f);
  quadObj.transform.setRotation(-90.0f, 0.0f, 0.0f);
  quadObj.transform.setScale(555.0f, 555.0f, 1.0f);
  _scene.objects.push_back(quadObj);

  // Ceiling
  quadObj.transform.setPosition(277.5f, 555.0f, -277.5f);
  quadObj.transform.setRotation(90.0f, 0.0f, 0.0f);
  _scene.objects.push_back(quadObj);

  // Back wall
  quadObj.transform.setPosition(277.5f, 277.5f, -555.0f);
  quadObj.transform.setRotation(0.0f, 0.0f, 0.0f);
  _scene.objects.push_back(quadObj);

  // Left wall
  quadObj.material = red;
  quadObj.transform.setPosition(0.0f, 277.5f, -277.5f);
  quadObj.transform.setRotation(0.0f, 90.0f, 0.0f);
  _scene.objects.push_back(quadObj);

  // Right wall
  quadObj.material = green;
  quadObj.transform.setPosition(555.0f, 277.5f, -277.5f);
  quadObj.transform.setRotation(0.0f, -90.0f, 0.0f);
  _scene.objects.push_back(quadObj);

  // Light
  quadObj.material = Material::light(25.0f);
  quadObj.transform.setPosition(277.5f, 554.0f, -277.5f);
  quadObj.transform.setRotation(90.0f, 0.0f, 0.0f);
  quadObj.transform.setScale(130.0f, 105.0f, 1.0f);
  _scene.objects.push_back(quadObj);

  Mesh cubeMesh;
  cubeMesh.vertices = {
      // front
      {{-0.5f, -0.5f, 0.5f}},
      {{0.5f, -0.5f, 0.5f}},
      {{0.5f, 0.5f, 0.5f}},
      {{-0.5f, 0.5f, 0.5f}},
      // back
      {{-0.5f, -0.5f, -0.5f}},
      {{0.5f, -0.5f, -0.5f}},
      {{0.5f, 0.5f, -0.5f}},
      {{-0.5f, 0.5f, -0.5f}},
  };
  cubeMesh.indices = {
      0, 1, 2, 0, 2, 3, // front
      1, 5, 6, 1, 6, 2, // right
      5, 4, 7, 5, 7, 6, // back
      4, 0, 3, 4, 3, 7, // left
      3, 2, 6, 3, 6, 7, // top
      4, 5, 1, 4, 1, 0, // bottom
  };

  Object cubeObj(cubeMesh, white);

  // Short block
  cubeObj.transform.setPosition(369.5f, 82.5f, -169.0f);
  cubeObj.transform.setRotation(0.0f, -18.0f, 0.0f);
  cubeObj.transform.setScale(165.0f, 165.0f, 165.0f);
  _scene.objects.push_back(cubeObj);

  // Tall block
  cubeObj.transform.setPosition(186.5f, 165.0f, -351.25f);
  cubeObj.transform.setRotation(0.0f, 15.0f, 0.0f);
  cubeObj.transform.setScale(165.0f, 330.0f, 165.0f);
  _scene.objects.push_back(cubeObj);

  // Move the camera
  _renderer->getCamera().setPosition(277.5f, 277.5f, 800.0f);
}

void SDLGraphicsProgram::initObjects() {
  // Add some spheres
  // _scene.spheres.push_back({{369.5f, 250.0f, -169.0f},
  //                           80.0f,
  //                           Material::metal(glm::vec3(1.0f), 0.0f)});
  _scene.spheres.push_back(
      {{186.5f, 420.0f, -351.25f}, 90.0f, Material::metal()});

  Object &bunny =
      _scene.objects.emplace_back("res/models/bunny/bunny_centered_fixed.obj");
  bunny.transform.setPosition(369.5f, 215.0f, -169.0f);
  bunny.transform.setScale(100.0f, 100.0f, 100.0f);

  // Textured cube
  // Object &texturedCube =
  //     _scene.objects.emplace_back("res/models/textured_cube/cube.obj");
  // texturedCube.transform.setPosition(369.5f, 250.0f, -200.0f);
  // texturedCube.transform.setScale(50.0f, 50.0f, 50.0f);
  // texturedCube.transform.setRotation(45.0f, 45.0f, 0.0f);
}

void SDLGraphicsProgram::initBuffers() {
  _vertexBuffer.createStorageBuffer(_scene.getVertices(), GL_STATIC_DRAW, 1);
  _gpuObjectBuffer.createStorageBuffer(_scene.bvh.getGpuObjects(),
                                       GL_STATIC_DRAW, 2);
  _bvhBuffer.createStorageBuffer(_scene.bvh.getNodes(), GL_STATIC_DRAW, 3);
  _materialBuffer.createStorageBuffer(_scene.materials, GL_STATIC_DRAW, 4);
}
