//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "RFFFirstRenderContextConfigurator.hpp"
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdSlopeAttribute.h"

namespace merutilm::rff2 {
    class SlopePipelineConfigurator final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_INPUT = 0;

        static constexpr uint32_t SET_ITERATION = 1;
        static constexpr uint32_t SET_SLOPE = 2;
        static constexpr uint32_t SET_RESOLUTION = 3;

    public:
        explicit
        SlopePipelineConfigurator(vkh::EngineRef engine,
                                  const uint32_t renderContextIndex,
                                  const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, primarySubpassIndex, "vk_slope.frag") {
        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex) override;

        void setResolution(const glm::uvec2 &swapchainExtent, float clarityMultiplier) const;

        void setSlope(const ShdSlopeAttribute &slope) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
