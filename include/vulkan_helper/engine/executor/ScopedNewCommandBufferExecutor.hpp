//
// Created by Merutilm on 2025-07-21.
//

#pragma once
#include <vulkan_helper/engine/cmd/CommandPool.hpp>
#include <vulkan_helper/engine/sync/Fence.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class ScopedNewCommandBufferExecutor final : public CoreHandler {
        CommandPool & commandPool;
        Fence * const fence;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    public:
        explicit ScopedNewCommandBufferExecutor(Core & core, CommandPool & commandPool, Fence * fence = VK_NULL_HANDLE);

        ~ScopedNewCommandBufferExecutor() override;

        ScopedNewCommandBufferExecutor(const ScopedNewCommandBufferExecutor &) = delete;

        ScopedNewCommandBufferExecutor &operator=(const ScopedNewCommandBufferExecutor &) = delete;

        ScopedNewCommandBufferExecutor(ScopedNewCommandBufferExecutor &&) = delete;

        ScopedNewCommandBufferExecutor &operator=(ScopedNewCommandBufferExecutor &&) = delete;

        [[nodiscard]] VkCommandBuffer getCommandBufferHandle() const { return commandBuffer; }

    private:
        void init() override;

        void cleanup() override;
    };
}
