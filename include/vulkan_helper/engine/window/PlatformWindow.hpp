//
// Created by Merutilm on 2026-02-07.
//

#pragma once
#include "EventSystem.hpp"
#include "MouseState.hpp"
#include "PlatformCheckboxMenuItemBase.hpp"
#include "PlatformMenuBase.hpp"
#include "PlatformMenuItemBase.hpp"
#include "WindowInitializerSettings.hpp"
#include "vulkan_helper/handle/Handler.hpp"


namespace merutilm::vkh {
    class PlatformWindow : Handler {

        GLFWwindow *window = nullptr;

    protected:
        bool mouseHovered = false;


        std::unordered_map<int, MouseState> mouseStates{};
        std::unordered_set<int> pressedKeys{};
        static constexpr int MOUSE_DRAG_THRESHOLD = 10;

    public:
        EventSystem eventSystem = {};
        WindowInitializerSettings initializerSettings;
        std::unique_ptr<PlatformMenuBase> menubar = nullptr;

        explicit PlatformWindow(WindowInitializerSettings &&settings);
        ~PlatformWindow() override;
        PlatformWindow(const PlatformWindow &) = delete;
        PlatformWindow &operator=(const PlatformWindow &) = delete;
        PlatformWindow(PlatformWindow &&) = delete;
        PlatformWindow &operator=(PlatformWindow &&) = delete;

        bool isKeyPressed(int key) const;

        void start();

        GLFWwindow *getWindow() const { return window; }

        VkExtent2D calcCurrentExtent() const;

        VkSurfaceKHR createSurface(VkInstance instance) const;

        static PlatformMenuBase &addMenu(PlatformMenuBase &menu, std::string name);
        static PlatformMenuItemBase &addMenuItem(PlatformMenuBase &menu, std::string name);
        static PlatformCheckboxMenuItemBase &addCheckboxMenuItem(PlatformMenuBase &menu, std::string name);

        bool canRenderNow() const;


        template<typename F>
            requires std::is_invocable_r_v<void, F>
        PlatformMenuItemBase &addMenuItemWithListener(PlatformMenuBase &menu, std::string name, F &&func) {
            auto &item = addMenuItem(menu, std::move(name));
            item.getOnClickListener().add(std::forward<F>(func));
            return item;
        }
        template<typename FGetter, typename F>
            requires std::is_invocable_r_v<void, F, bool>
        PlatformCheckboxMenuItemBase &addCheckboxMenuItemWithListener(PlatformMenuBase &menu, std::string name,
                                                                      FGetter &&getterFunc, F &&func) {
            auto &item = addCheckboxMenuItem(menu, std::move(name));
            item.setCheckboxStateGetter(getterFunc);
            item.getOnClickListener().add(std::forward<F>(func));
            return item;
        }

        static void processKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods);

        static void processMouseEnter(GLFWwindow *window, int entered);

        static void processMousePos(GLFWwindow *window, double xpos, double ypos);

        static void processMouseButton(GLFWwindow *window, int button, int action, int mods);

        static void processScroll(GLFWwindow *window, double xoffset, double yoffset);

        static void processWindowSize(GLFWwindow *window, int width, int height);

        static void processWindowMaximize(GLFWwindow *window, int maximized);

        static void processWindowMinimize(GLFWwindow *window, int minimized);

        static void processDropCallback(GLFWwindow *window, int pathCount, const char *paths[]);

        static void processWindowFocus(GLFWwindow *window, int focused);

        void setIcon() const;

    protected:
        void init() override;

        void cleanup() override;
    };
} // namespace merutilm::vkh
