//
// Created by Merutilm on 2026-05-10.
//

#pragma once
#ifdef _WIN32
#include "vulkan_helper/engine/window/PlatformCheckboxMenuItemBase.hpp"
#include "vulkan_helper/engine/window/PlatformMenuItemBase.hpp"
namespace merutilm::vkh {
    struct NativeCheckboxMenuItem final : PlatformCheckboxMenuItemBase {
        HMENU menu;
        UINT id;

        explicit NativeCheckboxMenuItem(std::wstring name, const PlatformMenuBase* parent);
        ~NativeCheckboxMenuItem() override;
        NativeCheckboxMenuItem(const NativeCheckboxMenuItem&) = delete;
        NativeCheckboxMenuItem& operator=(const NativeCheckboxMenuItem&) = delete;
        NativeCheckboxMenuItem(NativeCheckboxMenuItem&&) = delete;
        NativeCheckboxMenuItem& operator=(NativeCheckboxMenuItem&&) = delete;

    private:
        void init() override;
        void cleanup() override;
    };
}
#endif