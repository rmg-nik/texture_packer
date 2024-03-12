cmake_minimum_required(VERSION 3.20.0)
project(texture_packer VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
    GIT_TAG        "d79f8652510b8bd1f89c90be2ab65fc8940056eb" #the latest on 2024-03-12
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
    GIT_TAG        "277ae93c41314ba5f4c7444f37c4319cdf07e8cf" #version 1.90.4
)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)

FetchContent_MakeAvailable(SDL3 imgui)

# Build imgui static library
add_library(imgui STATIC 
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp 
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
)

target_link_libraries(imgui PRIVATE SDL3::SDL3)

target_include_directories(imgui
	PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

add_subdirectory(texture_packer)
