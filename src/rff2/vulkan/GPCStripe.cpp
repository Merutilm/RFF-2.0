//
// Created by Merutilm on 2025-08-15.
//

#include "GPCStripe.hpp"

#include "RCCFirst.hpp"
#include "../../vulkan_helper/util/ImageContextUtils.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/DescriptorUpdater.hpp"
#include "../attr/ShdStripeAttribute.h"

namespace merutilm::rff2 {
    void GPCStripe::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    void GPCStripe::setStripe(const ShdStripeAttribute &stripe) const {
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

        updateBufferForEachFrame([&stripeUBO](const uint32_t frameIndex) {
            stripeUBO.update(frameIndex);
        });
    }

    void GPCStripe::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorForEachFrame([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_STRIPE).queue(queue, frameIndex);
        });
    }

    void GPCStripe::windowResized() {
        const auto &input = engine.getRenderContextConfigurator<RCCFirst>().getImageContext(
            RCCFirst::TEMP_IMAGE_CONTEXT);
        const auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        auto &inputManager = inputDesc.getDescriptorManager();
        inputManager.get<vkh::InputAttachment>(BINDING_PREV_RESULT_INPUT).ctx = input;
        writeDescriptorForEachFrame([&inputDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            inputDesc.queue(queue, frameIndex);
        });
    }

    void GPCStripe::configurePushConstant(vkh::PipelineLayoutManagerRef  &pipelineLayoutManager) {
        //noop
    }

    void GPCStripe::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;

        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
    }
}
