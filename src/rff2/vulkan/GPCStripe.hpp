//
// Created by Merutilm on 2025-08-15.
//

#pragma once
#include "vulkan_helper/engine/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp"
#include "../settings/ShdStripeSettings.h"

namespace merutilm::rff2 {
    struct GPCStripe final : public vkh::GeneralPostProcessGraphicsPipelineConfigurator {

        static constexpr uint32_t SET_PREV_RESULT = 0;
        static constexpr uint32_t BINDING_PREV_RESULT_SAMPLER = 0;

        static constexpr uint32_t SET_ITERATION = 1;
        static constexpr uint32_t SET_STRIPE = 2;
        static constexpr uint32_t SET_TIME = 3;

        explicit GPCStripe(vkh::Engine &engine, vkh::WindowContext &wc) : GeneralPostProcessGraphicsPipelineConfigurator(
            engine, wc, "vk_stripe.frag") {
        }

        ~GPCStripe() override = default;

        GPCStripe(const GPCStripe &) = delete;

        GPCStripe &operator=(const GPCStripe &) = delete;

        GPCStripe(GPCStripe &&) = delete;

        GPCStripe &operator=(GPCStripe &&) = delete;


        void updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) override;

        void setStripe(const ShdStripeSettings &stripe) const;

        void pipelineInitialized() override;

        void renderContextRefreshed() override;

    protected:
        void configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) override;

        void configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) override;
    };
}
