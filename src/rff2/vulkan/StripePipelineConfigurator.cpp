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
        auto &input = engine.getRenderContext().configurator->getImageContext(
            RFFRenderContextConfigurator::PRIMARY_SUBPASS_RESULT_COLOR_IMAGE)[imageIndex];
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);
        auto &inputManager = inputDesc.getDescriptorManager();

        inputManager.get<std::unique_ptr<vkh::CombinedImageSampler> >(BINDING_PREV_RESULT_SAMPLER)->
                setImageContext(input);
        inputManager.get<vkh::ImageContext>(BINDING_PREV_RESULT_INPUT) = input;
        inputDesc.queue(queue, frameIndex);
    }

    void StripePipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        //noop
    }

    void StripePipelineConfigurator::configureDescriptors(std::vector<const vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        VkSamplerCreateInfo samplerInfo = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = engine.getCore().getPhysicalDevice().getPhysicalDeviceProperties().limits.
            maxSamplerAnisotropy,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0,
            .maxLod = 1,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE
        };

        auto descManager = std::make_unique<vkh::DescriptorManager>();
        auto sampler = std::make_unique<vkh::CombinedImageSampler>(
            engine.getCore(), engine.getRepositories().getRepository<vkh::SamplerRepo>()->pick(std::move(samplerInfo)));
        descManager->appendCombinedImgSampler(BINDING_PREV_RESULT_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
                                              std::move(sampler));
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

        auto queue = vkh::DescriptorUpdater::createQueue();
        for (uint32_t i = 0; i < engine.getCore().getPhysicalDevice().getMaxFramesInFlight(); ++i) {
            stripeUBO.update(i);
            stripeDesc.queue(queue, i);
        }
        vkh::DescriptorUpdater::write(engine.getCore().getLogicalDevice().getLogicalDeviceHandle(), queue);
    }
}
