//
// Created by Merutilm on 2025-07-29.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdPaletteAttribute.h"

namespace merutilm::rff2 {
    struct GPCIterationPalette final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_PALETTE = 1;
        static constexpr uint32_t SET_TIME = 2;

        uint32_t iterWidth = 0;
        uint32_t iterHeight = 0;

        GPCIterationPalette(vkh::EngineRef engine,
                                             const uint32_t renderContextIndex,
                                             const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, primarySubpassIndex,
            "vk_iteration_palette.frag") {
        };

        ~GPCIterationPalette() override = default;

        GPCIterationPalette(const GPCIterationPalette &) = delete;

        GPCIterationPalette &operator=(const GPCIterationPalette &) = delete;

        GPCIterationPalette(GPCIterationPalette &&) = delete;

        GPCIterationPalette &operator=(GPCIterationPalette &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void resetIterationBuffer(uint32_t width, uint32_t height);

        void resetIterationBuffer() const;

        void setIteration(uint32_t x, uint32_t y, double iteration) const;

        void setAllIterations(const std::vector<double> &iterations) const;

        void setMaxIteration(double maxIteration) const;

        void setPalette(const ShdPaletteAttribute &palette) const;

        void pipelineInitialized() override;

        void windowResized() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;

    };
}
