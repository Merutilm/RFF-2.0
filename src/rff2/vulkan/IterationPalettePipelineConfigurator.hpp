//
// Created by Merutilm on 2025-07-29.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class IterationPalettePipelineConfigurator final : public vkh::GeneralPostProcessPipelineConfigurator {
        static constexpr uint32_t SET_ITERATION = 0;
        static constexpr uint32_t SET_PALETTE = 1;

    public:
        IterationPalettePipelineConfigurator(const vkh::Engine &engine,
                                      uint32_t subpassIndex);

        ~IterationPalettePipelineConfigurator() override = default;

        IterationPalettePipelineConfigurator(const IterationPalettePipelineConfigurator &) = delete;

        IterationPalettePipelineConfigurator &operator=(const IterationPalettePipelineConfigurator &) = delete;

        IterationPalettePipelineConfigurator(IterationPalettePipelineConfigurator &&) = delete;

        IterationPalettePipelineConfigurator &operator=(IterationPalettePipelineConfigurator &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex, uint32_t width,
                         uint32_t height) override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) override;

    };
}
