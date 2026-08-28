//
// Created by Merutilm on 2025-07-21.
//

#pragma once
#include <vulkan_helper/engine/cmd/CommandPool.hpp>
#include <vulkan_helper/engine/sync/Fence.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>

#include "vulkan_helper/engine/cmd/CommandBuffer.hpp"

namespace merutilm::vkh {
    class ScopedNewCommandBufferExecutor final : public CoreHandler {
        CommandPool & commandPool;
        Fence * fence;
        std::unique_ptr<CommandBuffer> commandBuffer;

    public:
        explicit ScopedNewCommandBufferExecutor(Core & core, CommandPool & commandPool, Fence * fence = VK_NULL_HANDLE);

        ~ScopedNewCommandBufferExecutor() override;

        ScopedNewCommandBufferExecutor(const ScopedNewCommandBufferExecutor &) = delete;

        ScopedNewCommandBufferExecutor &operator=(const ScopedNewCommandBufferExecutor &) = delete;

        ScopedNewCommandBufferExecutor(ScopedNewCommandBufferExecutor &&) = delete;

        ScopedNewCommandBufferExecutor &operator=(ScopedNewCommandBufferExecutor &&) = delete;

        [[nodiscard]] CommandBuffer &getCommandBuffer() const { return *commandBuffer; }

    protected:
        void init() override;

        void cleanup() override;
    };
}
