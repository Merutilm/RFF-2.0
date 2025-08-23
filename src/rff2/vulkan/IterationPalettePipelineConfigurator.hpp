//
// Created by Merutilm on 2025-07-29.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessPipelineConfigurator.hpp"
#include "../attr/ShdPaletteAttribute.h"

namespace merutilm::rff2 {
    class IterationPalettePipelineConfigurator final : public vkh::GeneralPostProcessPipelineConfigurator {
        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_PALETTE = 1;
        static constexpr uint32_t SET_TIME = 2;

        uint32_t width = 0;
        uint32_t height = 0;

    public:
        IterationPalettePipelineConfigurator(const vkh::Engine &engine,
                                             const uint32_t subpassIndex): GeneralPostProcessPipelineConfigurator(
            engine, subpassIndex, "vk_iteration_palette.frag") {
        };

        ~IterationPalettePipelineConfigurator() override = default;

        IterationPalettePipelineConfigurator(const IterationPalettePipelineConfigurator &) = delete;

        IterationPalettePipelineConfigurator &operator=(const IterationPalettePipelineConfigurator &) = delete;

        IterationPalettePipelineConfigurator(IterationPalettePipelineConfigurator &&) = delete;

        IterationPalettePipelineConfigurator &operator=(IterationPalettePipelineConfigurator &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex, uint32_t width,
                         uint32_t height) override;

        void reloadIterationBuffer(uint32_t width, uint32_t height);

        void resetIterationBuffer() const;

        void setIteration(uint32_t x, uint32_t y, double iteration) const;

        void setAllIterations(const std::vector<double> &iterations) const;

        void setMaxIteration(double maxIteration) const;

        void setPalette(const ShdPaletteAttribute &palette) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) override;
    };
}
