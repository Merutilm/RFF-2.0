//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include <vulkan_helper/engine/configurator/PipelineConfigurator.hpp>


namespace merutilm::vkh {
    class RenderPassFullscreenRecorder final : public WindowContextHandler {
        const uint32_t renderContextIndex;
        const uint32_t frameIndex;
        const uint32_t swapchainImageIndex;

        explicit RenderPassFullscreenRecorder(WindowContext & wc, uint32_t renderContextIndex, uint32_t frameIndex,
                                              uint32_t swapchainImageIndex);

        ~RenderPassFullscreenRecorder() override;

        RenderPassFullscreenRecorder(const RenderPassFullscreenRecorder &) = delete;

        RenderPassFullscreenRecorder &operator=(const RenderPassFullscreenRecorder &) = delete;

        RenderPassFullscreenRecorder(RenderPassFullscreenRecorder &&) noexcept = delete;

        RenderPassFullscreenRecorder &operator=(RenderPassFullscreenRecorder &&) = delete;

    public:
        template<typename Configurator> requires std::is_base_of_v<RenderContextConfigurator, Configurator>
        static void cmdFullscreenRenderPass(WindowContext & wc, const uint32_t frameIndex, const uint32_t swapchainImageIndex, const std::vector<PipelineConfigurator *> shaderPrograms, std::vector<DescIndexPicker> && descIndices) {
            const auto renderPassRecorder = RenderPassFullscreenRecorder(
                wc, Configurator::CONTEXT_INDEX, frameIndex, swapchainImageIndex);
            renderPassRecorder.cmdMatchViewportAndScissor();
            renderPassRecorder.execute(frameIndex, shaderPrograms, std::move(descIndices));
        }

        template<typename Configurator> requires std::is_base_of_v<RenderContextConfigurator, Configurator>
        static void cmdFullscreenPresentOnlyRenderPass(WindowContext & wc, const uint32_t frameIndex, const uint32_t swapchainImageIndex, const std::vector<PipelineConfigurator *> shaderPrograms, std::vector<DescIndexPicker> && descIndices) {
            cmdFullscreenRenderPass<Configurator>(wc, frameIndex, swapchainImageIndex, shaderPrograms, std::move(descIndices));
        }

        template<typename Configurator> requires std::is_base_of_v<RenderContextConfigurator, Configurator>
        static void cmdFullscreenInternalRenderPass(WindowContext & wc, const uint32_t frameIndex, const std::vector<PipelineConfigurator *> shaderPrograms, std::vector<DescIndexPicker> && descIndices) {
            cmdFullscreenRenderPass<Configurator>(wc, frameIndex, UINT32_MAX,  shaderPrograms, std::move(descIndices));
        }


        void cmdMatchViewportAndScissor() const;

        void execute(uint32_t frameIndex, std::span<PipelineConfigurator * const> shaderPrograms, std::vector<DescIndexPicker> &&descIndices) const;

    private:
        void init() override;

        void cleanup() override;
    };
}
