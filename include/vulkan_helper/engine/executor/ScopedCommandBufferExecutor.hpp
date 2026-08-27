//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include <vulkan_helper/engine/Engine.hpp>
#include <vulkan_helper/handle/WindowContextHandler.hpp>

namespace merutilm::vkh {
    class ScopedCommandBufferExecutor final : public WindowContextHandler {
        const VkCommandBuffer commandBuffer;
        const VkFence fence;
        const VkSemaphore wait;
        const VkSemaphore signal;
    public:
        explicit ScopedCommandBufferExecutor(WindowContext & wc, VkCommandBuffer commandBuffer, VkFence fence, VkSemaphore wait, VkSemaphore signal);

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
