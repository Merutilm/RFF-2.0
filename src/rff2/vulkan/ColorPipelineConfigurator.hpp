//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "RFFFirstRenderContextConfigurator.hpp"
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdColorAttribute.h"

namespace merutilm::rff2 {
    class ColorPipelineConfigurator final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_INPUT = 0;

        static constexpr uint32_t SET_COLOR = 1;

    public:
        explicit ColorPipelineConfigurator(vkh::EngineRef engine,
                                           const uint32_t renderContextIndex,
                                           const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, primarySubpassIndex, "vk_color.frag") {
        }

        ~ColorPipelineConfigurator() override = default;

        ColorPipelineConfigurator(const ColorPipelineConfigurator &) = delete;

        ColorPipelineConfigurator(ColorPipelineConfigurator &&) = delete;

        ColorPipelineConfigurator &operator=(const ColorPipelineConfigurator &) = delete;

        ColorPipelineConfigurator &operator=(ColorPipelineConfigurator &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex) override;

        void setColor(const ShdColorAttribute &color) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
