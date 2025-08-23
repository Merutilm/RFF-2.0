//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "../../vulkan_helper/configurator/GeneralPostProcessPipelineConfigurator.hpp"
#include "../attr/ShdStripeAttribute.h"

namespace merutilm::rff2 {
    class StripePipelineConfigurator final : public vkh::GeneralPostProcessPipelineConfigurator {

        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_INPUT = 0;

        static constexpr uint32_t SET_ITERATION = 1;
        static constexpr uint32_t SET_STRIPE = 2;
        static constexpr uint32_t SET_TIME = 3;

    public:
        StripePipelineConfigurator(const vkh::Engine &engine,
                                   const uint32_t subpassIndex) : GeneralPostProcessPipelineConfigurator(
            engine, subpassIndex, "vk_stripe.frag") {
        }

        ~StripePipelineConfigurator() override = default;

        StripePipelineConfigurator(const StripePipelineConfigurator &) = delete;

        StripePipelineConfigurator &operator=(const StripePipelineConfigurator &) = delete;

        StripePipelineConfigurator(StripePipelineConfigurator &&) = delete;

        StripePipelineConfigurator &operator=(StripePipelineConfigurator &&) = delete;




        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex, uint32_t imageIndex, uint32_t width,
                         uint32_t height) override;

        void setStripe(const ShdStripeAttribute &stripe) const;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) override;

        void configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) override;
    };
}
