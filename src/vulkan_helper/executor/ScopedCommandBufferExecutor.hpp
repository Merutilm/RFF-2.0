//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "../handle/WindowContextHandler.hpp"
#include "../impl/Engine.hpp"

namespace merutilm::vkh {
    class ScopedCommandBufferExecutor final : public WindowContextHandler {
        const uint32_t frameIndex;
        const VkFence fence;
        const VkSemaphore imageAvailable;
        const VkSemaphore renderFinished;
    public:
        explicit ScopedCommandBufferExecutor(WindowContextRef wc, uint32_t frameIndex, VkFence fence, VkSemaphore imageAvailable, VkSemaphore renderFinished);

        ~ScopedCommandBufferExecutor() override;

        ScopedCommandBufferExecutor(const ScopedCommandBufferExecutor &) = delete;

        ScopedCommandBufferExecutor &operator=(const ScopedCommandBufferExecutor &) = delete;

        ScopedCommandBufferExecutor(ScopedCommandBufferExecutor &&) = delete;

        ScopedCommandBufferExecutor &operator=(ScopedCommandBufferExecutor &&) = delete;

        void init() override;

        void destroy() override;
    };
}
