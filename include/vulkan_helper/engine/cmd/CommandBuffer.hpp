//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/engine/cmd/CommandPool.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>
#include <vulkan_helper/core/Core.hpp>

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

    protected:
        void init() override;

        void cleanup() override;
    };

}
