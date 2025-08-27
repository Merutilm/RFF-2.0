//
// Created by Merutilm on 2025-07-21.
//

#pragma once
#include "Executor.hpp"

namespace merutilm::vkh {
    class ScopedCommandExecutor final : public Executor {
        CommandPoolRef commandPool;
        VkCommandBuffer commandBuffer = nullptr;

    public:
        explicit ScopedCommandExecutor(CoreRef core, CommandPoolRef commandPool);

        ~ScopedCommandExecutor() override;

        ScopedCommandExecutor(const ScopedCommandExecutor &) = delete;

        ScopedCommandExecutor &operator=(const ScopedCommandExecutor &) = delete;

        ScopedCommandExecutor(ScopedCommandExecutor &&) = delete;

        ScopedCommandExecutor &operator=(ScopedCommandExecutor &&) = delete;

        [[nodiscard]] VkCommandBuffer getCommandBufferHandle() const { return commandBuffer; }

    private:
        void begin() override;

        void end() override;
    };
}
