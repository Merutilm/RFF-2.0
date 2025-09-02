//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "RCC1.hpp"
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"

namespace merutilm::rff2 {
    class GPCPresent final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {
        static constexpr uint32_t SET_RESULT = 0;
        static constexpr uint32_t BINDING_RESULT_SAMPLER = 0;
        static constexpr uint32_t SET_RESOLUTION = 1;
    public:
        explicit GPCPresent(vkh::EngineRef engine,
                                           const uint32_t renderContextIndex,
                                           const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, primarySubpassIndex, "vk_present.frag") {
        }

        ~GPCPresent() override = default;

        GPCPresent(const GPCPresent &) = delete;

        GPCPresent(GPCPresent &&) = delete;

        GPCPresent &operator=(const GPCPresent &) = delete;

        GPCPresent &operator=(GPCPresent &&) = delete;

        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void pipelineInitialized() override;

        void windowResized() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;

    };
}
