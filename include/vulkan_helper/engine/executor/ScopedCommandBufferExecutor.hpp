//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include <vulkan_helper/engine/Engine.hpp>
#include <vulkan_helper/handle/WindowContextHandler.hpp>

namespace merutilm::vkh {
    class ScopedCommandBufferExecutor final : public WindowContextHandler {
        const uint32_t frameIndex;
        const VkFence fence;
        const VkSemaphore imageAvailable;
        const VkSemaphore renderFinished;
    public:
        explicit ScopedCommandBufferExecutor(WindowContext & wc, uint32_t frameIndex, VkFence fence, VkSemaphore imageAvailable, VkSemaphore renderFinished);

        ~ScopedCommandBufferExecutor() override;

        ScopedCommandBufferExecutor(const ScopedCommandBufferExecutor &) = delete;

        ScopedCommandBufferExecutor &operator=(const ScopedCommandBufferExecutor &) = delete;

        ScopedCommandBufferExecutor(ScopedCommandBufferExecutor &&) = delete;

        ScopedCommandBufferExecutor &operator=(ScopedCommandBufferExecutor &&) = delete;

    protected:
        void init() override;

        void cleanup() override;
    };
}
