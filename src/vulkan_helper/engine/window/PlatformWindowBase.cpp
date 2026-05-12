
//
// Created by Merutilm on 2026-02-07.

#include <vulkan_helper/base/logger.hpp>
#include <vulkan_helper/engine/window/PlatformWindowBase.hpp>


#ifdef _WIN32
#include <vulkan_helper/engine/window/win/NativeMenu.hpp>
#include "vulkan_helper/engine/window/win/NativeCheckboxMenuItem.hpp"
#include "vulkan_helper/engine/window/win/NativeMenuItem.hpp"
#elif __APPLE__
#include <vulkan_helper/engine/window/mac/NativeMenu.hpp>
#include "vulkan_helper/engine/window/mac/NativeCheckboxMenuItem.hpp"
#include "vulkan_helper/engine/window/mac/NativeMenuItem.hpp"
#elif __linux__
#include <vulkan_helper/engine/window/linux/NativeMenu.hpp>
#include "vulkan_helper/engine/window/linux/NativeCheckboxMenuItem.hpp"
#include "vulkan_helper/engine/window/linux/NativeMenuItem.hpp"
#endif

namespace merutilm::vkh {

    PlatformWindowBase::PlatformWindowBase(WindowInitializerSettings &&settings) : initializerSettings(std::move(settings)) { PlatformWindowBase::init(); }
    PlatformWindowBase::~PlatformWindowBase() { PlatformWindowBase::cleanup(); }

    void PlatformWindowBase::init() {
        //noop
    }

    void PlatformWindowBase::start() { eventSystem.applicationLifecycle.onStart.invoke(); }

    PlatformMenuBase &PlatformWindowBase::addMenu(PlatformMenuBase &menu, std::wstring name) {
        auto nm = std::make_unique<NativeMenu>(std::move(name), &menu);
        const auto &ref = menu.items.emplace_back(std::move(nm));
        return *ref;
    }
    PlatformMenuItemBase &PlatformWindowBase::addMenuItem(PlatformMenuBase &menu, std::wstring name) {
        auto nm = std::make_unique<NativeMenuItem>(std::move(name), &menu);
        const auto &ref = menu.items.emplace_back(std::move(nm));
        return static_cast<PlatformMenuItemBase &>(*ref);
    }
    PlatformCheckboxMenuItemBase &PlatformWindowBase::addCheckboxMenuItem(PlatformMenuBase &menu, std::wstring name) {
        auto nm = std::make_unique<NativeCheckboxMenuItem>(std::move(name), &menu);
        const auto &ref = menu.items.emplace_back(std::move(nm));
        return static_cast<PlatformCheckboxMenuItemBase &>(*ref);
    }

    KeyInput::Key PlatformWindowBase::findEquivalentKey(uint32_t nativeKey) {

        const auto result = keyMapper.find(nativeKey);
        if (result == keyMapper.end()) {
            logger::log("UNKNOWN key detected!! keyCode : {0}", nativeKey);
            return KeyInput::Key::KEY_UNKNOWN;
        }
        return result->second;
    }

    bool PlatformWindowBase::isKeyPressed(const KeyInput::Key key) const {
        return pressedKeys.contains(key);
    }

    void PlatformWindowBase::cleanup() {
        //noop
    }

} // namespace merutilm::vkh
