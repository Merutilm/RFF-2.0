//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/engine/cmd/CommandPool.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>
#include <vulkan_helper/core/Core.hpp>

#include "vulkan_helper/engine/sync/Fence.hpp"
#include "vulkan_helper/engine/sync/Semaphore.hpp"

namespace merutilm::vkh {
    class CommandBuffer final : public CoreHandler {
        VkCommandBuffer commandBuffer = {};
        CommandPool & commandPool;
    public:
        explicit CommandBuffer(Core & core, CommandPool & commandPool);

        ~CommandBuffer() override;

        CommandBuffer(const CommandBuffer &) = delete;

        CommandBuffer &operator=(const CommandBuffer &) = delete;

        CommandBuffer(CommandBuffer &&) = delete;

        CommandBuffer &operator=(CommandBuffer &&) = delete;

        [[nodiscard]] VkCommandBuffer getCommandBufferHandle() const {
            return commandBuffer;
        }

        [[nodiscard]] CommandPool &getCommandPool() const {
            return commandPool;
        }

        void begin() const;

        void end() const;

        void submit(const Fence *fence, const std::vector<const Semaphore *> &waitSemaphores, const std::vector<const Semaphore *> &signalSemaphores) const;

    protected:
        void init() override;

        void cleanup() override;
    };

}
