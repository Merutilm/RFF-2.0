//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include <vulkan_helper/engine/Engine.hpp>
#include <vulkan_helper/handle/WindowContextHandler.hpp>

#include "vulkan_helper/engine/cmd/CommandBuffer.hpp"

namespace merutilm::vkh {
    class ScopedCommandBufferExecutor final : public WindowContextHandler {
        const CommandBuffer &commandBuffer;
        const Fence &fence;
        const Semaphore *wait;
        const Semaphore *signal;
    public:
        explicit ScopedCommandBufferExecutor(WindowContext & wc, const CommandBuffer &commandBuffer, const Fence &fence, const Semaphore *wait, const Semaphore *signal);

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
