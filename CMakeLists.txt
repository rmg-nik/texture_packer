cmake_minimum_required(VERSION 3.20.0)

project(texture_packer VERSION 0.1.0 LANGUAGES C CXX)

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/ThirdParty/SDL/CMakeLists.txt")
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

set(SDL_SHARED OFF)
set(SDL3_SHARED OFF)
set(BUILD_SHARED_LIBS OFF)
set(SDL_STATIC ON)
set(SDL_HAPTIC OFF)
set(SDL_HIDAPI OFF)
set(SDL_JOYSTICK OFF)
set(SDL_JOYSTICK OFF)

add_subdirectory("ThirdParty/SDL")
add_subdirectory("ThirdParty/SDL_image")
add_subdirectory("ThirdParty/nlohmann_json")
add_subdirectory("ThirdParty/cxxopts")
add_subdirectory("ThirdParty/freetype")
add_subdirectory("ThirdParty/portable-file-dialogs")

set(imgui_SOURCE_DIR "ThirdParty/imgui")
# Build imgui static library
add_library(imgui STATIC 
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp 
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/misc/freetype/imgui_freetype.cpp
)

target_compile_definitions(imgui PUBLIC IMGUI_ENABLE_FREETYPE)
target_link_libraries(imgui PRIVATE SDL3::SDL3 freetype)

target_include_directories(imgui
	PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

#find_package(ImageMagick COMPONENTS Magick++ MagickCore MagickWand REQUIRED)

add_subdirectory(texture_packer)
add_subdirectory(app)
