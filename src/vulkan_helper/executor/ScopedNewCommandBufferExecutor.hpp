//
// Created by Merutilm on 2025-07-21.
//

#pragma once
#include "Executor.hpp"
#include "../impl/Engine.hpp"

namespace merutilm::vkh {
    class ScopedNewCommandBufferExecutor final : public Executor {
        CoreRef core;
        VkFence fenceHandle;
        CommandPoolRef commandPool;
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
        void begin() override;

        void end() override;
    };
}
