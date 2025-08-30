//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "RCCFirst.hpp"
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class GPCResample final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_RESAMPLE = 0;

        static constexpr uint32_t BINDING_RESAMPLE_SAMPLER = 0;
        static constexpr uint32_t BINDING_RESAMPLE_UBO = 1;

        static constexpr uint32_t TARGET_RESAMPLE_EXTENT = 0;
    public:
        explicit GPCResample(vkh::EngineRef engine,
                                           const uint32_t renderContextIndex,
                                           const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, primarySubpassIndex, "vk_resample.frag") {
        }

        ~GPCResample() override = default;

        GPCResample(const GPCResample &) = delete;

        GPCResample(GPCResample &&) = delete;

        GPCResample &operator=(const GPCResample &) = delete;

        GPCResample &operator=(GPCResample &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setTargetImageContext(const vkh::MultiframeImageContext &context, uint32_t frameIndex) const;

        void setResolution(const glm::uvec2 & newResolution) const;

        void pipelineInitialized() override;

        void windowResized() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
