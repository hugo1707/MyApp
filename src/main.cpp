// C++
#include <iostream>
#include <string>
#include <vector>

#include "GUI/Window.h"

int windowWidth = 1280, windowHeight = 720;

void draw(SDL_Window *window) {

  ImGui::Dummy(ImVec2(0.0f, 1.0f));
  ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "Platform");
  ImGui::Text("%s", SDL_GetPlatform());
  ImGui::Text("CPU cores: %d", SDL_GetCPUCount());
  ImGui::Text("RAM: %.2f GB", SDL_GetSystemRAM() / 1024.0f);

  // buttons and most other widgets return true when
  // clicked/edited/activated

  ImGui::SameLine();
  ImGui::Text("Hello world");

  ImGui::Text()
}

int main(int argc, char *argv[]) {

  try {
    Window w("Dear ImGui SDL", 1280, 720);

    w.set_draw_callback(draw);

    while (!w.should_close()) {
      w.render();

      // ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize);

      // ImGui::Text("Hello world");

      // ImGui::End();
    }

  } catch (std::exception ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
