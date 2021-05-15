#include "Window.h"

// STL
#include <iostream>
#include <stdexcept>

// SDL
#include <SDL.h>
#include <glad/glad.h>

// Dear ImGui
// #include "ImGUI/imgui-style.h"
#include "../ImGUI/imgui_impl_opengl3.h"
#include "../ImGUI/imgui_impl_sdl.h"

Window::Window(const std::string &title, int width, int height)
    : width(width), height(height), closing(false), draw_callback(nullptr) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("[ERROR] %s\n", SDL_GetError());
    throw_sdl_error();
  }

  // set OpenGL attributes
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  std::string glsl_version = "";
#ifdef __APPLE__
  // GL 3.2 Core + GLSL 150
  glsl_version = "#version 150";
  SDL_GL_SetAttribute( // required on Mac OS
      SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif __linux__
  // GL 3.2 Core + GLSL 150
  glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif _WIN32
  // GL 3.0 + GLSL 130
  glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif

  SDL_WindowFlags window_flags = (SDL_WindowFlags)(
      SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  window =
      SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, width, height, window_flags);
  // limit to which minimum size user can resize the window
  SDL_SetWindowMinimumSize(window, 500, 300);

  gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);

  // enable VSync
  SDL_GL_SetSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    throw std::runtime_error("Couldn not initialize glad");
  }
  glViewport(0, 0, width, height);

  // setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // setup Dear ImGui style
  ImGui::StyleColorsDark();

  // setup platform/renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

Window::~Window() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

bool Window::should_close() { return closing; }

void Window::render() {

  // colors are set in RGBA, but as float
  ImVec4 background = ImVec4(35 / 255.0f, 35 / 255.0f, 35 / 255.0f, 1.00f);

  glClearColor(background.x, background.y, background.z, background.w);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    // without it you won't have keyboard input and other things
    ImGui_ImplSDL2_ProcessEvent(&event);
    // you might also want to check io.WantCaptureMouse and
    // io.WantCaptureKeyboard before processing events

    switch (event.type) {
    case SDL_QUIT:
      closing = true;
      break;

    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        width = event.window.data1;
        height = event.window.data2;
        // std::cout << "[INFO] Window size: "
        //           << windowWidth
        //           << "x"
        //           << windowHeight
        //           << std::endl;
        glViewport(0, 0, width, height);
        break;
      }
      break;

    case SDL_KEYDOWN:
      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        closing = true;
        break;
      }
      break;
    }
  }

  // start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window);
  ImGui::NewFrame();

  if (draw_callback != nullptr) {
    draw_callback(window);
  }

  // rendering
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  SDL_GL_SwapWindow(window);
}
