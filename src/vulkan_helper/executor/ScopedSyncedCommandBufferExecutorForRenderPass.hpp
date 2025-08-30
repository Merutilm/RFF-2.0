//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "Executor.hpp"
#include "../impl/Engine.hpp"
#include "../struct/RenderPassProcess.hpp"

namespace merutilm::vkh {
    class ScopedSyncedCommandBufferExecutorForRenderPass final : public Executor {
        EngineRef engine;
        const uint32_t frameIndex;
        const RenderPassProcessTypeFlags renderPassProcessType;
    public:
        explicit ScopedSyncedCommandBufferExecutorForRenderPass(EngineRef engine, uint32_t frameIndex, RenderPassProcessTypeFlags renderPassProcessType);

        ~ScopedSyncedCommandBufferExecutorForRenderPass() override;

        ScopedSyncedCommandBufferExecutorForRenderPass(const ScopedSyncedCommandBufferExecutorForRenderPass &) = delete;

        ScopedSyncedCommandBufferExecutorForRenderPass &operator=(const ScopedSyncedCommandBufferExecutorForRenderPass &) = delete;

        ScopedSyncedCommandBufferExecutorForRenderPass(ScopedSyncedCommandBufferExecutorForRenderPass &&) = delete;

        ScopedSyncedCommandBufferExecutorForRenderPass &operator=(ScopedSyncedCommandBufferExecutorForRenderPass &&) = delete;

        void begin() override;

        void end() override;
    };
}
