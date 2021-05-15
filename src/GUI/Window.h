#pragma once

#include <functional>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "../ImGUI/imgui_impl_sdl.h"

#include "WindowException.h"

/*          Window management class
 *
 */
class Window {
public:
  using draw_callback_t = std::function<void(SDL_Window *)>;

public:
  Window(const std::string &title, int width, int height);
  ~Window();

  // Callback functions
  void set_draw_callback(draw_callback_t cb) { draw_callback = cb; }

  bool should_close();
  void render();

private:
  static void throw_sdl_error() { throw WindowException(SDL_GetError()); }

private:
  SDL_Window *window;
  SDL_GLContext gl_context;

  int width, height;
  bool closing;

  draw_callback_t draw_callback;

  // TTF_Font *font;

  // std::vector<std::pair<SDL_Texture *, SDL_Rect>> textures_pairs;
};
