cmake_minimum_required(VERSION 3.20.0)

project(texture_packer_app VERSION 0.1.0 LANGUAGES C CXX)

add_executable(${PROJECT_NAME} cli.cpp gui.cpp main.cpp)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_link_libraries(${PROJECT_NAME} 
    SDL3::SDL3 
    imgui 
    texture_packer_lib 
    cxxopts 
    portable_file_dialogs
    fmt::fmt
    )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "${PROJECT_BINARY_DIR}/assets"
     COMMENT "Copy assets")
