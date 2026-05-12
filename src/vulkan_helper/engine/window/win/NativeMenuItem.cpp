//
// Created by Merutilm on 2026-05-10.
//

#ifdef _WIN32
#include <vulkan_helper/engine/window/win/NativeMenuItem.hpp>

#include "vulkan_helper/engine/window/win/NativeMenu.hpp"
#include "vulkan_helper/engine/window/win/NativeMenuID.hpp"

namespace merutilm::vkh {

    NativeMenuItem::NativeMenuItem(std::wstring name, PlatformMenuBase* parent) : PlatformMenuItemBase(std::move(name), parent) {
        NativeMenuItem::init();
    }
    NativeMenuItem::~NativeMenuItem() {
        NativeMenuItem::cleanup();
    }

    void NativeMenuItem::init() {
        menu = CreateMenu();
        assert(parent);
        const HMENU parentHandle = dynamic_cast<const NativeMenu *>(parent)->getHandle();

        id = NativeMenuID::ID_MENUS + NativeMenuID::count++;
        AppendMenu(parentHandle, MF_STRING, id, name.data());
    }

    void NativeMenuItem::cleanup() {
        DestroyMenu(menu);
    }
} // namespace merutilm::vkh
#endif