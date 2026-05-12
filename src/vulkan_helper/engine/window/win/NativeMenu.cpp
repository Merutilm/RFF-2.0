//
// Created by Merutilm on 2026-05-10.
//

#ifdef _WIN32
#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/engine/window/win/NativeMenu.hpp>

namespace merutilm::vkh {

    NativeMenu::NativeMenu(std::wstring name, const PlatformMenuBase *parent) : PlatformMenuBase(std::move(name), parent) {
        NativeMenu::init();
    }
    NativeMenu::~NativeMenu() { NativeMenu::cleanup(); }
    void NativeMenu::init() {
        menu = CreateMenu();

        if (!parent)
            return;

        const HMENU parentHandle = dynamic_cast<const NativeMenu *>(parent)->menu;
        AppendMenu(parentHandle, MF_POPUP, reinterpret_cast<UINT_PTR>(menu), name.data());

    }
    void NativeMenu::cleanup() { DestroyMenu(menu); }
} // namespace merutilm::vkh
#endif
