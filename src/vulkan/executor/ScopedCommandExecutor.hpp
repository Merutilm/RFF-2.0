//
// Created by Merutilm on 2025-07-21.
//

#pragma once
#include "Executor.hpp"

namespace merutilm::mvk {
    class ScopedCommandExecutor final : public Executor {
        const CommandPool &commandPool;
        VkCommandBuffer commandBuffer = nullptr;

    public:
        explicit ScopedCommandExecutor(const Core &core, const CommandPool &commandPool);

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
