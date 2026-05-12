//
// Created by Merutilm on 2026-02-06.
//

#pragma once
#ifdef __linux__
#include "vulkan_helper/engine/window/PlatformWindowBase.hpp"

namespace merutilm::vkh {
    class NativeWindow final : public PlatformWindowBase {
        explicit NativeWindow(WindowInitializerSettings &&settings);

        ~NativeWindow() override;

        NativeWindow(const NativeWindow &) = delete;
        NativeWindow &operator=(const NativeWindow &) = delete;
        NativeWindow(NativeWindow &&) = delete;
        NativeWindow &operator=(NativeWindow &&) noexcept = delete;

    public:
        VkExtent2D calculateClientSizeFromMainWindow() const override;
        VkSurfaceKHR createSurface(VkInstance instance) override;
        void generateKeyMapper() override;
        bool canRenderable() override;
    };
}
#endif