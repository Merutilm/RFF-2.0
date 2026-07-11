//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/core/Instance.hpp>
#include <vulkan_helper/handle/Handler.hpp>

#include "PlatformWindow.hpp"


namespace merutilm::vkh {
    class Surface final : public Handler {

        Instance & instance;
        PlatformWindow * const window;

        VkSurfaceKHR surface = nullptr;

    public:
        explicit Surface(Instance & instance, PlatformWindow * window);

        ~Surface() override;

        Surface(const Surface &) = delete;

        Surface &operator=(const Surface &) = delete;

        Surface(Surface &&) = delete;

        Surface &operator=(Surface &&) = delete;

        [[nodiscard]] PlatformWindow * getTargetWindow() const { return window; }

        [[nodiscard]] VkSurfaceKHR getSurfaceHandle() const { return surface; }

    private:
        void init() override;

        void cleanup() override;
    };


}
