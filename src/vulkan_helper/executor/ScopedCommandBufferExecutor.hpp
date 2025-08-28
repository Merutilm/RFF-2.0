//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "Executor.hpp"
#include "../impl/Engine.hpp"

namespace merutilm::vkh {
    class ScopedCommandBufferExecutor final : public Executor {
        EngineRef engine;
        const uint32_t frameIndex;
    public:
        explicit ScopedCommandBufferExecutor(EngineRef engine, uint32_t frameIndex);

        ~ScopedCommandBufferExecutor() override;

        ScopedCommandBufferExecutor(const ScopedCommandBufferExecutor &) = delete;

        ScopedCommandBufferExecutor &operator=(const ScopedCommandBufferExecutor &) = delete;

        ScopedCommandBufferExecutor(ScopedCommandBufferExecutor &&) = delete;

        ScopedCommandBufferExecutor &operator=(ScopedCommandBufferExecutor &&) = delete;

        void begin() override;

        void end() override;
    };
}
