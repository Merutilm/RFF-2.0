//
// Created by Merutilm on 2026-05-11.
//

#include <vulkan_helper/engine/window/PlatformCheckboxMenuItemBase.hpp>

#include "vulkan_helper/engine/window/PlatformMenuItemBase.hpp"

namespace merutilm::vkh {

    PlatformCheckboxMenuItemBase::PlatformCheckboxMenuItemBase(std::string name, const PlatformMenuBase *parent) : PlatformMenuBase(std::move(name), parent) {
        PlatformCheckboxMenuItemBase::init();
    }
    PlatformCheckboxMenuItemBase::~PlatformCheckboxMenuItemBase() {
        PlatformCheckboxMenuItemBase::cleanup();
    }
    void PlatformCheckboxMenuItemBase::addKeybinding(Keybinding &&keybinding) {
        keybindings.emplace_back(std::move(keybinding));
    }
    Listener<bool> &PlatformCheckboxMenuItemBase::getOnClickListener() {
        return onClick;
    }
    void PlatformCheckboxMenuItemBase::init() {}
    void PlatformCheckboxMenuItemBase::cleanup() {}
} // namespace merutilm::vkh
