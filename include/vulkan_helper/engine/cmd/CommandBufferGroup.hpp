//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/engine/cmd/CommandPool.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>
#include <vulkan_helper/core/Core.hpp>

namespace merutilm::vkh {
    class CommandBufferGroup final : public CoreHandler {
        std::vector<VkCommandBuffer> commandBuffers = {};
        CommandPool & commandPool;
    public:
        explicit CommandBufferGroup(Core & core, CommandPool & commandPool);

        ~CommandBufferGroup() override;

        CommandBufferGroup(const CommandBufferGroup &) = delete;

        CommandBufferGroup &operator=(const CommandBufferGroup &) = delete;

        CommandBufferGroup(CommandBufferGroup &&) = delete;

        CommandBufferGroup &operator=(CommandBufferGroup &&) = delete;

        [[nodiscard]] VkCommandBuffer getCommandBufferHandle(const uint32_t frameIndex) const { return commandBuffers[frameIndex]; }

        [[nodiscard]] CommandPool &getCommandPool() const {
            return commandPool;
        }

    protected:
        void init() override;

        void cleanup() override;
    };

}
