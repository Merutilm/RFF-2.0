//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    
    struct GPCPresent final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator{
        static constexpr uint32_t SET_RESAMPLE = 0;

        static constexpr uint32_t BINDING_RESAMPLE_SAMPLER = 0;
        static constexpr uint32_t BINDING_RESAMPLE_UBO = 1;

        static constexpr uint32_t TARGET_RESAMPLE_UBO_EXTENT = 0;

        explicit GPCPresent(vkh::EngineRef engine,
                             const uint32_t renderContextIndex,
                             const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, primarySubpassIndex, "vk_resample.frag") {
        }

        ~GPCPresent() override = default;

        GPCPresent(const GPCPresent &) = delete;

        GPCPresent(GPCPresent &&) = delete;

        GPCPresent &operator=(const GPCPresent &) = delete;

        GPCPresent &operator=(GPCPresent &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setRescaledResolution(const glm::uvec2 &newResolution) const;

        void pipelineInitialized() override;

        void windowResized(uint32_t windowAttachmentIndex) override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };

}
