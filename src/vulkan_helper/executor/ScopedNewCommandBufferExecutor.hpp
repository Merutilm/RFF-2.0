//
// Created by Merutilm on 2025-07-21.
//

#pragma once
#include "../handle/CoreHandler.hpp"
#include "../impl/CommandPool.hpp"

namespace merutilm::vkh {
    class ScopedNewCommandBufferExecutor final : public CoreHandler {
        CommandPoolRef commandPool;
        VkFence fenceHandle;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    public:
        explicit ScopedNewCommandBufferExecutor(CoreRef core, CommandPoolRef commandPool, VkFence fenceHandle = VK_NULL_HANDLE);

        ~ScopedNewCommandBufferExecutor() override;

        ScopedNewCommandBufferExecutor(const ScopedNewCommandBufferExecutor &) = delete;

        ScopedNewCommandBufferExecutor &operator=(const ScopedNewCommandBufferExecutor &) = delete;

        ScopedNewCommandBufferExecutor(ScopedNewCommandBufferExecutor &&) = delete;

        ScopedNewCommandBufferExecutor &operator=(ScopedNewCommandBufferExecutor &&) = delete;

        [[nodiscard]] VkCommandBuffer getCommandBufferHandle() const { return commandBuffer; }

    private:
        void init() override;

        void destroy() override;
    };
}
