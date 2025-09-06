//
// Created by Merutilm on 2025-08-15.
//

#include "GPCStripe.hpp"

#include "RCC1.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/util/DescriptorUpdater.hpp"
#include "../attr/ShdStripeAttribute.h"
#include "../constants/VulkanWindowConstants.hpp"

namespace merutilm::rff2 {
    void GPCStripe::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    void GPCStripe::setStripe(const ShdStripeAttribute &stripe) const {
        using namespace SharedDescriptorTemplate;
        auto &stripeDesc = getDescriptor(SET_STRIPE);
        const auto &stripeUBO = *stripeDesc.get<vkh::Uniform>(0, DescStripe::BINDING_UBO_STRIPE);
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
        stripeUBO.update();

    }

    void GPCStripe::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorMF([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_STRIPE).queue(queue, frameIndex, {}, {DescStripe::TARGET_STRIPE_TYPE});
        });
    }

    void GPCStripe::windowResized() {
        auto &sic = *engine.getWindowContext(windowAttachmentIndex).sharedImageContext;
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);

        switch (windowAttachmentIndex) {
            case Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX: {
                const auto &input = sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY);
                inputDesc.get<vkh::InputAttachment>(0, BINDING_PREV_RESULT_INPUT).ctx = input;
                break;
            }
            case Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX: {
                //TODO : Video window
                break;
            }
            default: {
                //noop
            }
        }
        writeDescriptorMF([&inputDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            inputDesc.queue(queue, frameIndex, {}, {BINDING_PREV_RESULT_INPUT});
        });
    }

    void GPCStripe::configurePushConstant(vkh::PipelineLayoutManagerRef  &pipelineLayoutManager) {
        //noop
    }

    void GPCStripe::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;

        auto descManager = vkh::factory::create<vkh::DescriptorManager>();
        descManager->appendInputAttachment(BINDING_PREV_RESULT_INPUT, VK_SHADER_STAGE_FRAGMENT_BIT);

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
    }
}
