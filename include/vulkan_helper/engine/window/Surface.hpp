//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/core/Instance.hpp>
#include <vulkan_helper/handle/Handler.hpp>

#include "PlatformWindowBase.hpp"


namespace merutilm::vkh {
    class Surface final : public Handler {

        Instance & instance;
        PlatformWindowBase * const window;

        VkSurfaceKHR surface = nullptr;

    public:
        explicit Surface(Instance & instance, PlatformWindowBase * window);

        ~Surface() override;

        Surface(const Surface &) = delete;

        Surface &operator=(const Surface &) = delete;

        Surface(Surface &&) = delete;

        Surface &operator=(Surface &&) = delete;

        [[nodiscard]] PlatformWindowBase * getTargetWindow() const { return window; }

        [[nodiscard]] VkSurfaceKHR getSurfaceHandle() const { return surface; }

    private:
        void init() override;

        void cleanup() override;
    };


}
