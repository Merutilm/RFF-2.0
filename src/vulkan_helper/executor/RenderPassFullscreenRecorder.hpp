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
        const uint32_t swapchainImageIndex;

        explicit RenderPassFullscreenRecorder(EngineRef engine, uint32_t renderContextIndex, uint32_t frameIndex,
                                              uint32_t swapchainImageIndex);

        ~RenderPassFullscreenRecorder() override;

        RenderPassFullscreenRecorder(const RenderPassFullscreenRecorder &) = delete;

        RenderPassFullscreenRecorder &operator=(const RenderPassFullscreenRecorder &) = delete;

        RenderPassFullscreenRecorder(RenderPassFullscreenRecorder &&) = delete;

        RenderPassFullscreenRecorder &operator=(RenderPassFullscreenRecorder &&) = delete;

        template<typename Configurator> requires std::is_base_of_v<RenderContextConfiguratorAbstract, Configurator>
        static void cmdFullscreenRenderPass(EngineRef engine, const uint32_t frameIndex, const uint32_t swapchainImageIndex, const std::vector<PipelineConfigurator *> shaderPrograms, std::vector<DescIndexPicker> && descIndices) {
            const auto renderPassRecorder = RenderPassFullscreenRecorder(
                engine, Configurator::CONTEXT_INDEX, frameIndex, swapchainImageIndex);
            renderPassRecorder.cmdMatchViewportAndScissor();
            renderPassRecorder.execute(frameIndex, shaderPrograms, std::move(descIndices));
        }

    public:
        template<typename Configurator> requires std::is_base_of_v<RenderContextConfiguratorAbstract, Configurator>
        static void cmdFullscreenPresentOnlyRenderPass(EngineRef engine, const uint32_t frameIndex, const uint32_t swapchainImageIndex, const std::vector<PipelineConfigurator *> shaderPrograms, std::vector<DescIndexPicker> && descIndices) {
            cmdFullscreenRenderPass<Configurator>(engine, frameIndex, swapchainImageIndex, shaderPrograms, std::move(descIndices));
        }

        template<typename Configurator> requires std::is_base_of_v<RenderContextConfiguratorAbstract, Configurator>
        static void cmdFullscreenInternalRenderPass(EngineRef engine, const uint32_t frameIndex, const std::vector<PipelineConfigurator *> shaderPrograms, std::vector<DescIndexPicker> && descIndices) {
            cmdFullscreenRenderPass<Configurator>(engine, frameIndex, UINT32_MAX,  shaderPrograms, std::move(descIndices));
        }


        void cmdMatchViewportAndScissor() const;

        void execute(uint32_t frameIndex, std::span<PipelineConfigurator * const> shaderPrograms, std::vector<DescIndexPicker> &&descIndices) const;

    private:
        void begin() override;

        void end() override;
    };
}
