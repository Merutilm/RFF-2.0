//
// Created by Merutilm on 2025-08-15.
//

#include "StripePipelineConfigurator.hpp"

#include "RFFRenderContextConfigurator.hpp"
#include "../../vulkan_helper/util/ImageContextUtils.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/DescriptorUpdater.hpp"
#include "../settings/StripeSettings.h"

namespace merutilm::rff2 {
    void StripePipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                 const uint32_t imageIndex, const uint32_t width,
                                                 const uint32_t height) {
        auto &input = getRenderContextConfigurator<RFFRenderContextConfigurator>().getImageContext(
            RFFRenderContextConfigurator::PRIMARY_SUBPASS_RESULT_COLOR_IMAGE, imageIndex);
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        auto &inputManager = inputDesc.getDescriptorManager();

        inputManager.get<std::unique_ptr<vkh::CombinedImageSampler> >(BINDING_PREV_RESULT_SAMPLER)->
                setImageContext(input);
        inputManager.get<vkh::ImageContext>(BINDING_PREV_RESULT_INPUT) = input;
        inputDesc.queue(queue, frameIndex);
    }

    void StripePipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManagerRef  &pipelineLayoutManager) {
        //noop
    }

    void StripePipelineConfigurator::configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;

        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
    }

    void StripePipelineConfigurator::setStripeSettings(const StripeSettings &stripeSettings) const {
        using namespace SharedDescriptorTemplate;
        auto &stripeDesc = getDescriptor(SET_STRIPE);
        auto &stripeManager = stripeDesc.getDescriptorManager();
        const auto &stripeUBO = *stripeManager.get<std::unique_ptr<vkh::Uniform> >(
            DescStripe::BINDING_UBO_STRIPE);
        auto &stripeUBOHost = stripeUBO.getHostObject();
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_TYPE, static_cast<uint32_t>(stripeSettings.stripeType));
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_FIRST_INTERVAL,
                          stripeSettings.firstInterval);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_SECOND_INTERVAL,
                          stripeSettings.secondInterval);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_OPACITY, stripeSettings.opacity);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_OFFSET, stripeSettings.offset);
        stripeUBOHost.set(DescStripe::TARGET_STRIPE_ANIMATION_SPEED,
                          stripeSettings.animationSpeed);

        updateDescriptor([&stripeUBO, &stripeDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            stripeUBO.update(frameIndex);
            stripeDesc.queue(queue, frameIndex);
        });
    }
}
