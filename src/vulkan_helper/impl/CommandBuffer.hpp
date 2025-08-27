//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include "CommandPool.hpp"
#include "../handle/CoreHandler.hpp"
#include "Core.hpp"

namespace merutilm::vkh {
    class CommandBufferImpl final : public CoreHandler {
        std::vector<VkCommandBuffer> commandBuffers = {};
        CommandPoolRef commandPool;
    public:
        explicit CommandBufferImpl(CoreRef core, CommandPoolRef commandPool);

        ~CommandBufferImpl() override;

        CommandBufferImpl(const CommandBufferImpl &) = delete;

        CommandBufferImpl &operator=(const CommandBufferImpl &) = delete;

        CommandBufferImpl(CommandBufferImpl &&) = delete;

        CommandBufferImpl &operator=(CommandBufferImpl &&) = delete;

        [[nodiscard]] VkCommandBuffer getCommandBufferHandle(const uint32_t frameIndex) const { return commandBuffers[frameIndex]; }

    private:
        void init() override;

        void destroy() override;
    };
    using CommandBuffer = std::unique_ptr<CommandBufferImpl>;
    using CommandBufferPtr = CommandBufferImpl *;
    using CommandBufferRef = CommandBufferImpl &;
}
