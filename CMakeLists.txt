cmake_minimum_required(VERSION 3.31)
project(RFF)

include_directories(include)
link_directories(lib)

set(CMAKE_CXX_STANDARD 20)

add_executable(RFF src/rff/ui/main.cpp src/glad.c src/rff/opengl/GLShaderLoader.cpp
        src/rff/opengl/GLRenderer.cpp
        src/rff/opengl/GLRenderer.h
)

target_link_libraries(RFF PRIVATE opengl32)
target_link_libraries(RFF PRIVATE glfw3)