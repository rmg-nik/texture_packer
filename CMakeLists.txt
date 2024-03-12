cmake_minimum_required(VERSION 3.20.0)
project(texture_packer VERSION 0.1.0 LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
    GIT_TAG        "d79f8652510b8bd1f89c90be2ab65fc8940056eb" #the latest on 2024-03-12
)

set(SDL_SHARED OFF)
set(SDL_STATIC ON)

FetchContent_MakeAvailable(SDL3)

add_subdirectory(texture_packer)
