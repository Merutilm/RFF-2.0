
//
// Created by Merutilm on 2026-02-07.

#include <vulkan_helper/base/logger.hpp>
#include <vulkan_helper/engine/window/PlatformWindow.hpp>

#ifdef VKH_USE_IMGUI
#include "imgui.h"
#endif

namespace merutilm::vkh {

    PlatformWindow::PlatformWindow(WindowInitializerSettings &&settings) : initializerSettings(std::move(settings)) {
        PlatformWindow::init();
    }
    PlatformWindow::~PlatformWindow() { PlatformWindow::cleanup(); }

    void PlatformWindow::start() {
        using namespace std::chrono;
        startTime = high_resolution_clock::now();
        eventSystem.applicationLifecycle.onStart.invoke();
        updateTime();
        auto started = getTime();
        while (!glfwWindowShouldClose(window)) {

            glfwPollEvents();
            updateTime();
            const auto now = getTime();
            const auto elapsed = now - started;


            if (elapsed > 1.0f / initializerSettings.framerate) {
                if (canRenderNow()) {
                    started = now;
                    eventSystem.applicationLifecycle.onUpdate.invoke();
                }
            }
        }
        eventSystem.applicationLifecycle.onQuit.invoke();
    }


    VkExtent2D PlatformWindow::calcCurrentExtent() const {
        int w;
        int h;
        glfwGetWindowSize(window, &w, &h);
        return {static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
    }

    VkSurfaceKHR PlatformWindow::createSurface(const VkInstance instance) const {
        VkSurfaceKHR surface;
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw exception_init("failed to create surface!");
        }
        return surface;
    }


    PlatformMenuBase &PlatformWindow::addMenu(PlatformMenuBase &menu, std::string name) {
        auto nm = std::make_unique<PlatformMenuBase>(std::move(name), &menu);
        const auto &ref = menu.items.emplace_back(std::move(nm));
        return *ref;
    }
    PlatformMenuItemBase &PlatformWindow::addMenuItem(PlatformMenuBase &menu, std::string name) {
        auto nm = std::make_unique<PlatformMenuItemBase>(std::move(name), &menu);
        const auto &ref = menu.items.emplace_back(std::move(nm));
        return static_cast<PlatformMenuItemBase &>(*ref);
    }
    PlatformCheckboxMenuItemBase &PlatformWindow::addCheckboxMenuItem(PlatformMenuBase &menu, std::string name) {
        auto nm = std::make_unique<PlatformCheckboxMenuItemBase>(std::move(name), &menu);
        const auto &ref = menu.items.emplace_back(std::move(nm));
        return static_cast<PlatformCheckboxMenuItemBase &>(*ref);
    }

    bool PlatformWindow::canRenderNow() const {
        auto [width, height] = calcCurrentExtent();

        return !glfwGetWindowAttrib(window, GLFW_ICONIFIED) && glfwGetWindowAttrib(window, GLFW_VISIBLE) && width > 0 &&
               height > 0;
    }

    bool PlatformWindow::isKeyPressed(const int key) const { return pressedKeys.contains(key); }

    void PlatformWindow::processKeyInput(GLFWwindow *window, const int key, int scancode, const int action, int mods) {

#ifdef VKH_USE_IMGUI
        if (ImGui::GetIO().WantCaptureKeyboard)
            return;
#endif
        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS) {
            pwb->eventSystem.keyboard.onKeyDown.invoke(key);
            pwb->pressedKeys.insert(key);
        } else if (action == GLFW_RELEASE) {
            pwb->eventSystem.keyboard.onKeyUp.invoke(key);
            pwb->pressedKeys.erase(key);
        }
    }

    void PlatformWindow::processMouseEnter(GLFWwindow *window, const int entered) {
#ifdef VKH_USE_IMGUI
        if (ImGui::GetIO().WantCaptureMouse)
            return;
#endif
        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        if (entered == GLFW_TRUE) {
            pwb->mouseHovered = true;
            pwb->eventSystem.mouse.onMouseEnter.invoke();
        } else {
            pwb->mouseHovered = false;
            pwb->eventSystem.mouse.onMouseExit.invoke();
        }
    }

    void PlatformWindow::processMousePos(GLFWwindow *window, double xpos, double ypos) {
#ifdef VKH_USE_IMGUI
        if (ImGui::GetIO().WantCaptureMouse)
            return;
#endif

        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        const int ix = static_cast<int>(xpos);
        const int iy = static_cast<int>(ypos);

        pwb->eventSystem.mouse.onMouseMove.invoke(ix, iy);
        for (auto &[fst, snd]: pwb->mouseStates) {
            const int pdx = snd.pressedX - ix;
            const int pdy = snd.pressedY - iy;

            if (!snd.isDragging && pdx * pdx + pdy * pdy > MOUSE_DRAG_THRESHOLD * MOUSE_DRAG_THRESHOLD) {
                pwb->eventSystem.mouseDrag.onMouseDragStart.invoke(fst, ix, iy);
                snd.isDragging = true;
            }

            if (snd.isDragging) {
                const int ddx = ix - snd.lastX;
                const int ddy = iy - snd.lastY;
                pwb->eventSystem.mouseDrag.onMouseDrag.invoke(fst, ix, iy, ddx, ddy);
                snd.lastX = ix;
                snd.lastY = iy;
            }
        }
    }

    void PlatformWindow::processMouseButton(GLFWwindow *window, int button, int action, int mods) {
#ifdef VKH_USE_IMGUI
        if (ImGui::GetIO().WantCaptureMouse)
            return;
#endif

        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        double x;
        double y;
        glfwGetCursorPos(window, &x, &y);

        const int ix = static_cast<int>(x);
        const int iy = static_cast<int>(y);

        if (action == GLFW_PRESS) {
            pwb->eventSystem.mouse.onMouseDown.invoke(button, ix, iy);
            pwb->mouseStates.emplace(button, MouseState(ix, iy));
        } else if (action == GLFW_RELEASE) {
            pwb->eventSystem.mouse.onMouseUp.invoke(button, ix, iy);
            const auto nh = pwb->mouseStates.extract(button);
            if (!nh.empty()) {
                if (nh.mapped().isDragging) {
                    pwb->eventSystem.mouseDrag.onMouseDragEnd.invoke(button, ix, iy);
                }
            }
        }
    }

    void PlatformWindow::processScroll(GLFWwindow *window, double xoffset, double yoffset) {
#ifdef VKH_USE_IMGUI
        if (ImGui::GetIO().WantCaptureMouse)
            return;
#endif

        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        pwb->eventSystem.mouseWheel.onMouseScroll.invoke(static_cast<float>(yoffset));
    }

    void PlatformWindow::processWindowSize(GLFWwindow *window, int width, int height) {
        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        pwb->eventSystem.resize.onResize.invoke(width, height);
    }

    void PlatformWindow::processWindowMaximize(GLFWwindow *window, int maximized) {
        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        if (maximized == GLFW_TRUE) {
            pwb->eventSystem.window.onMaximize.invoke();
        } else {
            pwb->eventSystem.window.onRestore.invoke();
        }
    }

    void PlatformWindow::processWindowMinimize(GLFWwindow *window, int minimized) {
        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        if (minimized == GLFW_TRUE) {
            pwb->eventSystem.window.onMinimize.invoke();
        } else {
            pwb->eventSystem.window.onRestore.invoke();
        }
    }

    void PlatformWindow::processDropCallback(GLFWwindow *window, const int pathCount, const char *paths[]) {

        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        std::vector<std::string> pathString;
        for (int i = 0; i < pathCount; i++) {
            pathString.emplace_back(paths[i]);
        }
        pwb->eventSystem.dnd.onFileDrop.invoke(pathString);
    }

    void PlatformWindow::processWindowFocus(GLFWwindow *window, int focused) {
        const auto pwb = static_cast<PlatformWindow *>(glfwGetWindowUserPointer(window));
        if (focused == GLFW_TRUE) {
            pwb->eventSystem.focus.onFocusGained.invoke();
        } else {
            pwb->eventSystem.focus.onFocusLost.invoke();
        }
    }

    void PlatformWindow::setIcon() const {
        if (initializerSettings.icon.empty())
            return;

        int width, height, channels;
        if (unsigned char *pixels = stbi_load(initializerSettings.icon.data(), &width, &height, &channels, 4)) {
            GLFWimage image;
            image.width = width;
            image.height = height;
            image.pixels = pixels;
            glfwSetWindowIcon(window, 1, &image);
            stbi_image_free(pixels);
        }
    }
    void PlatformWindow::updateTime() {

        using namespace std::chrono;
        const time_point<high_resolution_clock> currentTime = high_resolution_clock::now();
        time = std::chrono::duration_cast<duration<float>>(currentTime - startTime).count();
    }



    void PlatformWindow::init() {
        window = glfwCreateWindow(initializerSettings.widthInfo.first, initializerSettings.heightInfo.first,
                                  initializerSettings.name.c_str(), nullptr, nullptr);


        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, processKeyInput);
        glfwSetCursorEnterCallback(window, processMouseEnter);
        glfwSetCursorPosCallback(window, processMousePos);
        glfwSetMouseButtonCallback(window, processMouseButton);
        glfwSetScrollCallback(window, processScroll);
        glfwSetWindowSizeCallback(window, processWindowSize);
        glfwSetWindowMaximizeCallback(window, processWindowMaximize);
        glfwSetWindowIconifyCallback(window, processWindowMinimize);
        glfwSetDropCallback(window, processDropCallback);
        glfwSetWindowFocusCallback(window, processWindowFocus);
        setIcon();
    }


    void PlatformWindow::cleanup() { glfwDestroyWindow(window); }

} // namespace merutilm::vkh
