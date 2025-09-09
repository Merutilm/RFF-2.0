//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdStripeAttribute.h"

namespace merutilm::rff2 {
    struct GPCStripe final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {

        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_SAMPLER = 0;

        static constexpr uint32_t SET_ITERATION = 1;
        static constexpr uint32_t SET_STRIPE = 2;
        static constexpr uint32_t SET_TIME = 3;

        explicit GPCStripe(vkh::EngineRef engine, const uint32_t windowContextIndex,
                                   const uint32_t renderContextIndex,
                                   const uint32_t primarySubpassIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, windowContextIndex, renderContextIndex, primarySubpassIndex, "vk_stripe.frag") {
        }

        ~GPCStripe() override = default;

        GPCStripe(const GPCStripe &) = delete;

        GPCStripe &operator=(const GPCStripe &) = delete;

        GPCStripe(GPCStripe &&) = delete;

        GPCStripe &operator=(GPCStripe &&) = delete;


        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setStripe(const ShdStripeAttribute &stripe) const;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
