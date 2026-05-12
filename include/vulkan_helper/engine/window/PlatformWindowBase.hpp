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
    class PlatformWindowBase : Handler {

    protected:
        bool controlKeyPressed = false;
        bool altKeyPressed = false;
        bool shiftKeyPressed = false;
        bool mouseHovered = false;
        bool isResizing = false;
        MouseState leftMouse = {};
        MouseState middleMouse = {};
        MouseState rightMouse = {};

        std::unordered_map<uint32_t, KeyInput::Key> keyMapper = {};
        std::unordered_set<KeyInput::Key> pressedKeys = {};

        static constexpr int MOUSE_DRAG_THRESHOLD = 10;

        int wheelMoved = 0;

    public:
        EventSystem eventSystem = {};
        WindowInitializerSettings initializerSettings;
        std::unique_ptr<PlatformMenuBase> menubar = nullptr;

        explicit PlatformWindowBase(WindowInitializerSettings &&settings);
        ~PlatformWindowBase() override;
        PlatformWindowBase(const PlatformWindowBase &) = delete;
        PlatformWindowBase &operator=(const PlatformWindowBase &) = delete;
        PlatformWindowBase(PlatformWindowBase &&) = delete;
        PlatformWindowBase &operator=(PlatformWindowBase &&) = delete;


        KeyInput::Key findEquivalentKey(uint32_t nativeKey);

        virtual void start();

        virtual VkSurfaceKHR createSurface(VkInstance instance) = 0;

        virtual void generateKeyMapper() = 0;

        virtual bool canRenderable() = 0;

        virtual void getMousePosition(int *x, int *y) = 0;

        [[nodiscard]] virtual VkExtent2D calculateClientSizeFromMainWindow() const = 0;

        [[nodiscard]] bool isKeyPressed(KeyInput::Key key) const;
        [[nodiscard]] bool isControlKeyPressed() const { return controlKeyPressed; }
        [[nodiscard]] bool isAltKeyPressed() const { return altKeyPressed; }
        [[nodiscard]] bool isShiftKeyPressed() const { return shiftKeyPressed; }
        [[nodiscard]] const MouseState &getLeftMouseState() const { return leftMouse; }
        [[nodiscard]] const MouseState &getMiddleMouseState() const { return middleMouse; }
        [[nodiscard]] const MouseState &getRightMouseState() const { return rightMouse; }

        virtual PlatformMenuBase &addMenu(PlatformMenuBase &menu, std::wstring name);
        virtual PlatformMenuItemBase &addMenuItem(PlatformMenuBase &menu, std::wstring name);
        virtual PlatformCheckboxMenuItemBase &addCheckboxMenuItem(PlatformMenuBase &menu, std::wstring name);


        template<typename F> requires std::is_invocable_r_v<void, F>
        PlatformMenuItemBase &addMenuItemWithListener(PlatformMenuBase &menu, std::wstring name, F&& func) {
            auto &item = addMenuItem(menu, std::move(name));
            item.getOnClickListener().add(std::forward<F>(func));
            return item;
        }
        template<typename FGetter, typename F> requires std::is_invocable_r_v<void, F, bool>
        PlatformCheckboxMenuItemBase &addCheckboxMenuItemWithListener(PlatformMenuBase &menu, std::wstring name, FGetter&& getterFunc, F&& func) {
            auto &item = addCheckboxMenuItem(menu, std::move(name));
            item.setCheckboxStateGetter(getterFunc);
            item.getOnClickListener().add(std::forward<F>(func));
            return item;
        }

        void init() override;

        void cleanup() override;
    };
} // namespace merutilm::vkh
