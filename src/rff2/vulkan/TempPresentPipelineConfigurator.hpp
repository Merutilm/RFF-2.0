//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "RFFFirstRenderContextConfigurator.hpp"
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class TempPresentPipelineConfigurator final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_INPUT = 0;
    public:
        explicit TempPresentPipelineConfigurator(vkh::EngineRef engine,
                                           const uint32_t renderContextIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, RFFFirstRenderContextConfigurator::SUBPASS_TEMP_PRESENT_INDEX, "vk_temp_present.frag") {
        }

        ~TempPresentPipelineConfigurator() override = default;

        TempPresentPipelineConfigurator(const TempPresentPipelineConfigurator &) = delete;

        TempPresentPipelineConfigurator(TempPresentPipelineConfigurator &&) = delete;

        TempPresentPipelineConfigurator &operator=(const TempPresentPipelineConfigurator &) = delete;

        TempPresentPipelineConfigurator &operator=(TempPresentPipelineConfigurator &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex) override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
