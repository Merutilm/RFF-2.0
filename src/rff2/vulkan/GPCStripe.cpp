//
// Created by Merutilm on 2025-08-15.
//

#include "GPCStripe.hpp"

#include "RCC1.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/repo/GlobalSamplerRepo.hpp"
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

    void GPCStripe::renderContextRefreshed() {
        auto &sic = wc.getSharedImageContext();
        auto &samplerDesc = getDescriptor(SET_PREV_RESULT);

        switch (wc.getAttachmentIndex()) {
            case Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX: {
                const auto &sampler = sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY);
                samplerDesc.get<vkh::CombinedImageSampler>(0, BINDING_PREV_RESULT_SAMPLER)->setImageContextMF(sampler);
                break;
            }
            case Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX: {
                const auto &sampler = sic.getImageContextMF(SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_SECONDARY);
                samplerDesc.get<vkh::CombinedImageSampler>(0, BINDING_PREV_RESULT_SAMPLER)->setImageContextMF(sampler);
                break;
            }
            default: {
                //noop
            }
        }
        writeDescriptorMF([&samplerDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            samplerDesc.queue(queue, frameIndex, {}, {BINDING_PREV_RESULT_SAMPLER});
        });
    }

    void GPCStripe::configurePushConstant(vkh::PipelineLayoutManagerRef  &pipelineLayoutManager) {
        //noop
    }

    void GPCStripe::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;

        auto descManager = vkh::factory::create<vkh::DescriptorManager>();
        vkh::SamplerRef sampler = pickFromGlobalRepository<vkh::GlobalSamplerRepo, vkh::SamplerRef>(
            VkSamplerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
                .mipLodBias = 0,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0,
                .maxLod = 0,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_TRUE
            });
        descManager->appendCombinedImgSampler(BINDING_PREV_RESULT_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, vkh::factory::create<vkh::CombinedImageSampler>(wc.core, sampler, true));
        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
    }
}
