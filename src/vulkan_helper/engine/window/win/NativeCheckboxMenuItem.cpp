//
// Created by Merutilm on 2026-05-10.
//

#include "vulkan_helper/engine/window/win/NativeCheckboxMenuItem.hpp"
#include "vulkan_helper/engine/window/win/NativeMenu.hpp"
#include "vulkan_helper/engine/window/win/NativeMenuID.hpp"

namespace merutilm::vkh {

    NativeCheckboxMenuItem::NativeCheckboxMenuItem(std::wstring name, const PlatformMenuBase *parent) : PlatformCheckboxMenuItemBase(std::move(name), parent){
        NativeCheckboxMenuItem::init();
    }
    NativeCheckboxMenuItem::~NativeCheckboxMenuItem() {
        NativeCheckboxMenuItem::cleanup();
    }
    void NativeCheckboxMenuItem::init() {
        menu = CreateMenu();
        assert(parent);
        const HMENU parentHandle = dynamic_cast<const NativeMenu *>(parent)->getHandle();

        id = NativeMenuID::ID_MENUS + NativeMenuID::count++;
        AppendMenu(parentHandle, 0, id, name.data());
    }

    void NativeCheckboxMenuItem::cleanup() {
        DestroyMenu(menu);
    }
} // namespace merutilm::vkh
