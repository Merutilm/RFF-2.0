//
// Created by Merutilm on 2025-08-15.
//

#include "GPCStripe.hpp"

#include "../settings/ShdStripeSettings.h"
#include "SharedImageContextIndices.hpp"
#include "desc/SharedDescriptorTemplate.hpp"
#include "vulkan_helper/engine/repo/GlobalSamplerRepo.hpp"
#include "vulkan_helper/util/DescriptorUpdater.hpp"


namespace merutilm::rff2 {
    void GPCStripe::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }


    void GPCStripe::pipelineInitialized() {
        //noop
    }

    void GPCStripe::renderContextRefreshed() {
        auto &sic = wc.getSharedImageContext();
        auto &samplerDesc = getDescriptor(SET_PREV_RESULT);
        const auto &sampler = sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY);
        samplerDesc.get<vkh::CombinedImageSampler>(0, BINDING_PREV_RESULT_SAMPLER).setImageContextMF(sampler);

        writeDescriptorMF([&samplerDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            samplerDesc.queue(queue, frameIndex, {}, {BINDING_PREV_RESULT_SAMPLER});
        });
    }

    void GPCStripe::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        //noop
    }

    void GPCStripe::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;

        auto descManager = vkh::DescriptorManager();
        vkh::Sampler &sampler = pickFromGlobalRepository<vkh::GlobalSamplerRepo, vkh::Sampler &>(
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
        descManager.appendCombinedImgSampler(BINDING_PREV_RESULT_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, std::make_unique<vkh::CombinedImageSampler>(wc.core, sampler, true));
        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescStripe>(SET_STRIPE, descriptors);
        appendDescriptor<DescTime>(SET_TIME, descriptors);
    }
}
