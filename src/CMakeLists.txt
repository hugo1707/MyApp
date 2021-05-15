cmake_minimum_required(VERSION 3.14)

project(MyApp VERSION 1.0.0)

set (CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(headers Network/ResourceLoader.h GUI/Window.h GUI/WindowException.h)

set(sources main.cpp Network/ResourceLoader.cpp GUI/Window.cpp)


# Add ImGUI

set(
    imgui_files 
    "imconfig.h" 
    "imgui.h" 
    "imgui_internal.h"
    "imstb_rectpack.h"
    "imstb_textedit.h"
    "imstb_truetype.h"
    "imgui.cpp"
    "imgui_tables.cpp"
    "imgui_widgets.cpp" 
    "imgui_demo.cpp" 
    "imgui_draw.cpp"

    # OpenGL 3
    "imgui_impl_opengl3.h"
    "imgui_impl_opengl3.cpp"

    # SDL
    "imgui_impl_sdl.h"
    "imgui_impl_sdl.cpp"
    )

list(TRANSFORM imgui_files PREPEND "ImGUI/")

add_executable(MainExe ${headers} ${sources} ${imgui_files})


target_include_directories(MainExe PRIVATE "glad/include/")
include_directories("glad/include/")

# Add libraries

find_package(OpenGL REQUIRED)
target_link_libraries(MainExe PRIVATE ${OPENGL_gl_LIBRARY})

add_library("glad" "glad/src/glad.c")
target_link_libraries(MainExe PRIVATE "glad")

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(MainExe PRIVATE SDL2::SDL2main SDL2::SDL2-static)

# find_package(sdl2-ttf CONFIG REQUIRED)
# target_link_libraries(MainExe PRIVATE SDL2::SDL2_ttf)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(MainExe PRIVATE fmt::fmt)

find_package(OpenSSL REQUIRED)

target_link_libraries(MainExe PRIVATE ${OPENSSL_LIBRARIES})

find_package(Boost COMPONENTS regex REQUIRED)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif ()

target_link_libraries(MainExe PRIVATE ${Boost_LIBRARIES})
