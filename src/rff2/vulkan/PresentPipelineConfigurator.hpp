//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "RFFFirstRenderContextConfigurator.hpp"
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class PresentPipelineConfigurator final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_RESULT = 0;
        static constexpr uint32_t BINDING_RESULT_SAMPLER = 0;
        static constexpr uint32_t SET_RESOLUTION = 1;
    public:
        explicit PresentPipelineConfigurator(vkh::EngineRef engine,
                                           const uint32_t renderContextIndex,
                                           const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, primarySubpassIndex, "vk_present.frag") {
        }

        ~PresentPipelineConfigurator() override = default;

        PresentPipelineConfigurator(const PresentPipelineConfigurator &) = delete;

        PresentPipelineConfigurator(PresentPipelineConfigurator &&) = delete;

        PresentPipelineConfigurator &operator=(const PresentPipelineConfigurator &) = delete;

        PresentPipelineConfigurator &operator=(PresentPipelineConfigurator &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex) override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
