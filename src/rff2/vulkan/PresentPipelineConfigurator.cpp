//
// Created by Merutilm on 2025-08-27.
//

#include "PresentPipelineConfigurator.hpp"

#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void PresentPipelineConfigurator::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex,
                                                  const uint32_t imageIndex) {
        auto &context = engine.getRenderContext(RFFFirstRenderContextConfigurator::CONTEXT_INDEX).getConfigurator()->
                getImageContext(
                    RFFFirstRenderContextConfigurator::RESULT_IMAGE_CONTEXT);
        const auto &resultDesc = getDescriptor(SET_RESULT);
        auto &resultManager = resultDesc.getDescriptorManager();
        resultManager.get<vkh::CombinedMultiframeImageSampler>(BINDING_RESULT_SAMPLER)->setImageContext(context);
        resultDesc.queue(queue, frameIndex, imageIndex);
    }

    void PresentPipelineConfigurator::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void PresentPipelineConfigurator::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        auto descManager = vkh::Factory::create<vkh::DescriptorManager>();
        vkh::SamplerRef sampler = pickFromRepository<vkh::SamplerRepo, vkh::SamplerRef, VkSamplerCreateInfo &&>(
            VkSamplerCreateInfo{
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
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0,
                .maxLod = 0,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE
            });
        auto combinedSampler = vkh::Factory::create<vkh::CombinedMultiframeImageSampler>(engine.getCore(), sampler);
        descManager->appendCombinedMultiframeImgSampler(BINDING_RESULT_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        std::move(combinedSampler));
        appendUniqueDescriptor(SET_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescResolution>(SET_RESOLUTION, descriptors);
    }
}
