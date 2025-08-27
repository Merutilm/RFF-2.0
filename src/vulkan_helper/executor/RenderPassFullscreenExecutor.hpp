//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include "Executor.hpp"

namespace merutilm::vkh {
    class RenderPassFullscreenExecutor final : public Executor {
        EngineRef engine;
        const uint32_t renderContextIndex;
        const uint32_t frameIndex;
        const uint32_t imageIndex;

    public:
        explicit RenderPassFullscreenExecutor(EngineRef engine, uint32_t renderContextIndex, uint32_t frameIndex, uint32_t imageIndex);

        ~RenderPassFullscreenExecutor() override;

        RenderPassFullscreenExecutor(const RenderPassFullscreenExecutor &) = delete;

        RenderPassFullscreenExecutor &operator=(const RenderPassFullscreenExecutor &) = delete;

        RenderPassFullscreenExecutor(RenderPassFullscreenExecutor &&) = delete;

        RenderPassFullscreenExecutor &operator=(RenderPassFullscreenExecutor &&) = delete;


    private:
        void begin() override;

        void end() override;

    };
}
