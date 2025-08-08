//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan/vulkan_core.h>

#include "Instance.hpp"
#include "GraphicsContextWindow.hpp"
#include "../handle/Handler.hpp"

namespace merutilm::mvk {
    class Surface final : public Handler {

        const Instance &instance;
        const GraphicsContextWindow &window;

        VkSurfaceKHR surface = nullptr;

    public:
        explicit Surface(const Instance &instance, const GraphicsContextWindow &window);

        ~Surface() override;

        Surface(const Surface &) = delete;

        Surface &operator=(const Surface &) = delete;

        Surface(Surface &&) = delete;

        Surface &operator=(Surface &&) = delete;

        [[nodiscard]] const GraphicsContextWindow &getTargetWindow() const { return window; }

        [[nodiscard]] VkSurfaceKHR getSurfaceHandle() const { return surface; }

    private:
        void init() override;

        void destroy() override;
    };
}
