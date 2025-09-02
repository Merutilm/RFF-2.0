//
// Created by Merutilm on 2025-08-31.
//

#include "GPCLinearInterpolation.hpp"

#include "RCCSecondary.hpp"
#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void GPCLinearInterpolation::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        //noop
    }

    void GPCLinearInterpolation::setLinearInterpolation(const bool use) const {
        using namespace SharedDescriptorTemplate;
        auto &interDesc = getDescriptor(SET_LINEAR_INTERPOLATION);
        const auto &interUBO = *interDesc.get<vkh::Uniform>(0, DescLinearInterpolation::BINDING_UBO_LINEAR_INTERPOLATION);
        auto &interUBOHost = interUBO.getHostObject();
        interUBOHost.set<bool>(DescLinearInterpolation::TARGET_LINEAR_INTERPOLATION_USE, use);

        updateBufferForEachFrame([&interUBO](const uint32_t frameIndex) {
            interUBO.update(frameIndex);
        });
    }

    void GPCLinearInterpolation::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorForEachFrame([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_LINEAR_INTERPOLATION).queue(queue, frameIndex, {}, {DescBloom::BINDING_UBO_BLOOM});
        });
    }

    void GPCLinearInterpolation::windowResized() {
        const auto &sample = engine.getRenderContextConfigurator<RCCSecondary>().getImageContext(
           RCCSecondary::RESULT_IMAGE_CONTEXT);
        auto &samplerDesc = getDescriptor(SET_PREV_RESULT);
        samplerDesc.get<vkh::CombinedMultiframeImageSampler>(0, BINDING_PREV_RESULT_SAMPLER)->setImageContext(sample);

        writeDescriptorForEachFrame([&samplerDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            samplerDesc.queue(queue, frameIndex, {}, {BINDING_PREV_RESULT_SAMPLER});
        });
    }

    void GPCLinearInterpolation::configurePushConstant(
        vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void GPCLinearInterpolation::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
        vkh::SamplerRef sampler = pickFromRepository<vkh::SamplerRepo, vkh::SamplerRef>(
            VkSamplerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = VK_FILTER_NEAREST,
                .minFilter = VK_FILTER_NEAREST,
                .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
                .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
                .mipLodBias = 0,
                .anisotropyEnable = VK_FALSE,
                .maxAnisotropy = 0,
                .compareEnable = VK_FALSE,
                .compareOp = VK_COMPARE_OP_ALWAYS,
                .minLod = 0,
                .maxLod = 0,
                .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
                .unnormalizedCoordinates = VK_FALSE,
            });
        auto descManager = vkh::factory::create<vkh::DescriptorManager>();

        descManager->appendCombinedMultiframeImgSampler(BINDING_PREV_RESULT_SAMPLER,
                                                        VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        vkh::factory::create<vkh::CombinedMultiframeImageSampler>(
                                                            engine.getCore(), sampler));
        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescLinearInterpolation>(SET_LINEAR_INTERPOLATION, descriptors);
    }
}
