//
// Created by Merutilm on 7/10/26.
//

#pragma once
#include <GLFW/glfw3.h>
namespace merutilm::rff2 {
    struct CursorManager {
        GLFWwindow *window;
        GLFWcursor *arrowCursor = nullptr;
        GLFWcursor *handCursor = nullptr;
        GLFWcursor *crosshairCursor = nullptr;

        explicit CursorManager(GLFWwindow *window) {
            this->window = window;
            arrowCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
            handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
            crosshairCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        }

        ~CursorManager() {
            glfwSetCursor(window, nullptr);
            glfwDestroyCursor(arrowCursor);
            glfwDestroyCursor(handCursor);
            glfwDestroyCursor(crosshairCursor);
        }

        CursorManager(CursorManager const &) = delete;
        CursorManager &operator=(CursorManager const &) = delete;
        CursorManager(CursorManager &&) = delete;
        CursorManager &operator=(CursorManager &&) = delete;
    };
} // namespace merutilm::rff2
