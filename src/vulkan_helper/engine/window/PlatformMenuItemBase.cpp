//
// Created by Merutilm on 2026-05-10.
//
#include <vulkan_helper/engine/window/PlatformMenuItemBase.hpp>
namespace merutilm::vkh {

    PlatformMenuItemBase::PlatformMenuItemBase(std::wstring name, PlatformMenuBase* parent) : PlatformMenuBase(std::move(name), parent) {
        PlatformMenuItemBase::init();
    }
    PlatformMenuItemBase::~PlatformMenuItemBase() {
        PlatformMenuItemBase::cleanup();
    }
    void PlatformMenuItemBase::addKeybinding(Keybinding &&keybinding) {
        keybindings.emplace_back(std::move(keybinding));
    }
    Listener<> &PlatformMenuItemBase::getOnClickListener() {
        return onClick;
    }
    void PlatformMenuItemBase::init() {

    }
    void PlatformMenuItemBase::cleanup() {

    }
} // namespace merutilm::vkh
