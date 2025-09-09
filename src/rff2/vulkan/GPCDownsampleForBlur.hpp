//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    struct GPCDownsampleForBlur final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {

        static constexpr uint32_t SET_RESAMPLE = 0;
        static constexpr uint32_t BINDING_RESAMPLE_SAMPLER = 0;

        static constexpr uint32_t DESC_COUNT_RESAMPLE_IMAGE = 2;
        static constexpr uint32_t DESC_INDEX_RESAMPLE_IMAGE_FOG = 0;
        static constexpr uint32_t DESC_INDEX_RESAMPLE_IMAGE_BLOOM = 1;

        static constexpr uint32_t BINDING_RESAMPLE_UBO = 1;
        static constexpr uint32_t TARGET_RESAMPLE_UBO_EXTENT = 0;

        explicit GPCDownsampleForBlur(vkh::EngineRef engine, const uint32_t windowContextIndex,
                             const uint32_t renderContextIndex,
                             const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, windowContextIndex, renderContextIndex, primarySubpassIndex, "vk_resample.frag") {
        }

        ~GPCDownsampleForBlur() override = default;

        GPCDownsampleForBlur(const GPCDownsampleForBlur &) = delete;

        GPCDownsampleForBlur(GPCDownsampleForBlur &&) = delete;

        GPCDownsampleForBlur &operator=(const GPCDownsampleForBlur &) = delete;

        GPCDownsampleForBlur &operator=(GPCDownsampleForBlur &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setRescaledResolution(uint32_t descIndex, const glm::uvec2 &newResolution) const;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
