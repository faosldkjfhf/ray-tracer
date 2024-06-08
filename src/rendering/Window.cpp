#include "rendering/Window.hpp"

#include "core/Error.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl3.h"

#include <iostream>

Window::Window(int width, int height, const std::string &title)
    : _width(width), _height(height), _title(title) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
    exit(1);
  }

  // Setup the OpenGL Context
  // Use OpenGL 4.1 core or greater
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  // We want to request a double buffer for smooth updating.
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  // Debug context
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

  _window =
      SDL_CreateWindow(_title.c_str(), _width, _height, SDL_WINDOW_OPENGL);
  if (!_window) {
    std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    exit(1);
  }

  _context = SDL_GL_CreateContext(_window);
  if (!_context) {
    std::cerr << "Failed to create OpenGL context: " << SDL_GetError()
              << std::endl;
    exit(1);
  }

  // Initialize GLAD Library
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    exit(1);
  }

  // Init debug output
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(err::glDebugOutput, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr,
                          GL_TRUE);
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup ImGui backends
  ImGui_ImplSDL3_InitForOpenGL(_window, _context);
  ImGui_ImplOpenGL3_Init();
}

Window::~Window() {
  SDL_GL_DeleteContext(_context);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}
