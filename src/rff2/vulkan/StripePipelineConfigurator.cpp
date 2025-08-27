//
// Created by Merutilm on 2025-08-15.
//

#include "StripePipelineConfigurator.hpp"

#include "RFFFirstRenderContextConfigurator.hpp"
#include "../../vulkan_helper/util/ImageContextUtils.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/DescriptorUpdater.hpp"
#include "../attr/ShdStripeAttribute.h"

namespace merutilm::rff2 {
    void StripePipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                 const uint32_t imageIndex) {
        auto &input = getRenderContextConfigurator<RFFFirstRenderContextConfigurator>().getImageContext(
            RFFFirstRenderContextConfigurator::TEMP_IMAGE_CONTEXT);
        const auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        auto &inputManager = inputDesc.getDescriptorManager();
        inputManager.get<vkh::MultiframeImageContext>(BINDING_PREV_RESULT_INPUT) = input;
        inputDesc.queue(queue, frameIndex, imageIndex);
    }

    void StripePipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManagerRef  &pipelineLayoutManager) {
        //noop
    }

    void StripePipelineConfigurator::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;

        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
    }

    void StripePipelineConfigurator::setStripe(const ShdStripeAttribute &stripe) const {
        using namespace SharedDescriptorTemplate;
        const auto &stripeDesc = getDescriptor(SET_STRIPE);
        auto &stripeManager = stripeDesc.getDescriptorManager();
        const auto &stripeUBO = *stripeManager.get<vkh::Uniform>(
            DescStripe::BINDING_UBO_STRIPE);
        auto &stripeUBOHost = stripeUBO.getHostObject();
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_TYPE, static_cast<uint32_t>(stripe.stripeType));
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_FIRST_INTERVAL,
                          stripe.firstInterval);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_SECOND_INTERVAL,
                          stripe.secondInterval);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_OPACITY, stripe.opacity);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_OFFSET, stripe.offset);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_ANIMATION_SPEED,
                          stripe.animationSpeed);

        writeDescriptorForEachFrame([&stripeUBO, &stripeDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t inFlightIndex) {
            stripeUBO.update(inFlightIndex);
            stripeDesc.queue(queue, inFlightIndex, inFlightIndex);
        });
    }
}
