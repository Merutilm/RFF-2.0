//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "RFFFirstRenderContextConfigurator.hpp"
#include "../../vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../attr/ShdStripeAttribute.h"

namespace merutilm::rff2 {
    class StripePipelineConfigurator final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {

        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_INPUT = 0;

        static constexpr uint32_t SET_ITERATION = 1;
        static constexpr uint32_t SET_STRIPE = 2;
        static constexpr uint32_t SET_TIME = 3;

    public:
        StripePipelineConfigurator(vkh::EngineRef engine,
                                   const uint32_t renderContextIndex) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, renderContextIndex, RFFFirstRenderContextConfigurator::SUBPASS_STRIPE_INDEX, "vk_stripe.frag") {
        }

        ~StripePipelineConfigurator() override = default;

        StripePipelineConfigurator(const StripePipelineConfigurator &) = delete;

        StripePipelineConfigurator &operator=(const StripePipelineConfigurator &) = delete;

        StripePipelineConfigurator(StripePipelineConfigurator &&) = delete;

        StripePipelineConfigurator &operator=(StripePipelineConfigurator &&) = delete;




        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex) override;

        void setStripe(const ShdStripeAttribute &stripe) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) override;
    };
}
