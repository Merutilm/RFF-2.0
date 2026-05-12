//
// Created by Merutilm on 2026-05-10.
//

#pragma once
#ifdef _WIN32
#include "vulkan_helper/engine/window/PlatformMenuItemBase.hpp"
namespace merutilm::vkh {
    struct NativeMenuItem final : PlatformMenuItemBase {
        HMENU menu;
        UINT id;

        explicit NativeMenuItem(std::wstring name, PlatformMenuBase* parent);
        ~NativeMenuItem() override;
        NativeMenuItem(const NativeMenuItem&) = delete;
        NativeMenuItem& operator=(const NativeMenuItem&) = delete;
        NativeMenuItem(NativeMenuItem&&) = delete;
        NativeMenuItem& operator=(NativeMenuItem&&) = delete;


    private:
        void init() override;
        void cleanup() override;
    };
}
#endif