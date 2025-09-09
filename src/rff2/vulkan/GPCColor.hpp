//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdColorAttribute.h"

namespace merutilm::rff2 {
    struct GPCColor final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_INPUT = 0;

        static constexpr uint32_t SET_COLOR = 1;

        explicit GPCColor(vkh::EngineRef engine, const uint32_t windowContextIndex,
                                           const uint32_t renderContextIndex,
                                           const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, windowContextIndex, renderContextIndex, primarySubpassIndex, "vk_color.frag") {
        }

        ~GPCColor() override = default;

        GPCColor(const GPCColor &) = delete;

        GPCColor(GPCColor &&) = delete;

        GPCColor &operator=(const GPCColor &) = delete;

        GPCColor &operator=(GPCColor &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setColor(const ShdColorAttribute &color) const;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
