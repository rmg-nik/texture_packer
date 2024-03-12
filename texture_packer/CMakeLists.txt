cmake_minimum_required(VERSION 3.20.0)

add_executable(texture_packer main.cpp)

target_link_libraries(texture_packer SDL3::SDL3 imgui)