//
// Created by Merutilm on 2025-08-30.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdBloomAttribute.h"

namespace merutilm::rff2 {
    struct GPCBloom final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_BLOOM_CANVAS = 0;
        static constexpr uint32_t BINDING_BLOOM_CANVAS_ORIGINAL = 0;
        static constexpr uint32_t BINDING_BLOOM_CANVAS_BLURRED = 1;

        static constexpr uint32_t SET_BLOOM = 1;

        explicit GPCBloom(vkh::WindowContextRef wc, const uint32_t renderContextIndex,
                                           const uint32_t
                                           subpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            wc, renderContextIndex, subpassIndex, "vk_bloom.frag") {
        }

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setBloom(const ShdBloomAttribute &bloom) const;

        void pipelineInitialized() override;

        void windowResized() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;


        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
