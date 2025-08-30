//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdBloomAttribute.h"

namespace merutilm::rff2 {
    class GPCBloomThreshold final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_BLOOM_THRESHOLD = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_INPUT = 0;

        static constexpr uint32_t SET_BLOOM = 1;
        static constexpr uint32_t SET_RESOLUTION = 2;

    public:
        explicit GPCBloomThreshold(vkh::EngineRef engine, const uint32_t renderContextIndex,
                                                    const uint32_t
                                                    subpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, subpassIndex, "vk_bloom_threshold.frag") {
        }

        ~GPCBloomThreshold() override = default;

        GPCBloomThreshold(const GPCBloomThreshold &) = delete;

        GPCBloomThreshold &operator=(const GPCBloomThreshold &) = delete;

        GPCBloomThreshold(GPCBloomThreshold &&) = delete;

        GPCBloomThreshold &operator=(GPCBloomThreshold &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void pipelineInitialized() override;

        void windowResized() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
