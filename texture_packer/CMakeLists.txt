cmake_minimum_required(VERSION 3.20.0)

project(texture_packer_lib VERSION 0.1.0 LANGUAGES C CXX)

set(SOURCES
  "src/atlas.cpp"
  "src/image_info.cpp"
  "src/image.cpp"
  "src/texture_packer.cpp"
  "src/utils.cpp")

add_library(${PROJECT_NAME} ${SOURCES})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json SDL3::SDL3 SDL3_image::SDL3_image)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()