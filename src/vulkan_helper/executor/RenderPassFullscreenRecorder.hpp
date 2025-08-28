//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include "Executor.hpp"
#include "../configurator/PipelineConfigurator.hpp"


namespace merutilm::vkh {
    class RenderPassFullscreenRecorder final : public Executor {
        EngineRef engine;
        const uint32_t renderContextIndex;
        const uint32_t frameIndex;
        const uint32_t imageIndex;

    public:
        explicit RenderPassFullscreenRecorder(EngineRef engine, uint32_t renderContextIndex, uint32_t frameIndex,
                                              uint32_t imageIndex);

        ~RenderPassFullscreenRecorder() override;

        RenderPassFullscreenRecorder(const RenderPassFullscreenRecorder &) = delete;

        RenderPassFullscreenRecorder &operator=(const RenderPassFullscreenRecorder &) = delete;

        RenderPassFullscreenRecorder(RenderPassFullscreenRecorder &&) = delete;

        RenderPassFullscreenRecorder &operator=(RenderPassFullscreenRecorder &&) = delete;

        template<typename Configurator> requires std::is_base_of_v<RenderContextConfiguratorAbstract, Configurator>
        static void cmdFullscreenRenderPass(EngineRef engine, const std::vector<PipelineConfigurator *> shaderPrograms, const uint32_t frameIndex, const uint32_t imageIndex) {
            const auto renderPassRecorder = RenderPassFullscreenRecorder(
                engine, Configurator::CONTEXT_INDEX, frameIndex, imageIndex);
            renderPassRecorder.cmdMatchViewportAndScissor();
            renderPassRecorder.execute(shaderPrograms, frameIndex);
        }


        void cmdMatchViewportAndScissor() const;

        void execute(std::span<PipelineConfigurator * const> shaderPrograms, uint32_t frameIndex) const;

    private:
        void begin() override;

        void end() override;
    };
}
