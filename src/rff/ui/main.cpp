
#include <iostream>
#include <Windows.h>

#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "../opengl/GLShaderLoader.h"
#include "imgui/imgui.h"


constexpr short WIDTH = 1280;
constexpr short HEIGHT = 720;
constexpr short FPS = 60;



void framebuffer_size_callback(const int width, const int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW!\n";
        glfwTerminate();
        return -1;
    }


    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "RFF", nullptr, nullptr);
    if (!window) {
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)framebuffer_size_callback);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD!\n";

        glfwTerminate();
        return -1;
    }

    GLShaderLoader loader("default_vertex", "default_fragment");


    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glfwPollEvents();
        glfwSwapBuffers(window);
        Sleep(long(1000.0 / FPS));
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



