//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdSlopeAttribute.h"

namespace merutilm::rff2 {
    struct GPCSlope final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_SAMPLER = 0;

        static constexpr uint32_t SET_ITERATION = 1;
        static constexpr uint32_t SET_SLOPE = 2;

        explicit GPCSlope(vkh::EngineRef engine, const uint32_t windowContextIndex,
                                  const uint32_t renderContextIndex,
                                  const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, windowContextIndex, renderContextIndex, primarySubpassIndex, "vk_slope.frag") {
        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setSlope(const ShdSlopeAttribute &slope) const;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
