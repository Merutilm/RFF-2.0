//
// Created by Merutilm on 2025-08-30.
//

#include "GPCBloom.hpp"

#include "RCCDownsampleForBlur.hpp"
#include "RCCFog.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "GPCSlope.hpp"

namespace merutilm::rff2 {
    void GPCBloom::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        //no operation
    }

    void GPCBloom::setBloom(const ShdBloomAttribute &bloom) const {
        using namespace SharedDescriptorTemplate;
        auto &bloomDesc = getDescriptor(SET_BLOOM);
        auto &bloomUBO = *bloomDesc.get<vkh::Uniform>(0, DescBloom::BINDING_UBO_BLOOM);
        auto &bloomUBOHost = bloomUBO.getHostObject();

        if (bloomUBO.isLocked()) {
            bloomUBO.unlock(engine.getCommandPool());
        }

        bloomUBOHost.set<float>(DescBloom::TARGET_BLOOM_THRESHOLD, bloom.threshold);
        bloomUBOHost.set<float>(DescBloom::TARGET_BLOOM_RADIUS, bloom.radius);
        bloomUBOHost.set<float>(DescBloom::TARGET_BLOOM_SOFTNESS, bloom.softness);
        bloomUBOHost.set<float>(DescBloom::TARGET_BLOOM_INTENSITY, bloom.intensity);

        updateBufferForEachFrame([&bloomUBO](const uint32_t frameIndex) {
            bloomUBO.update(frameIndex);
        });

        bloomUBO.lock(engine.getCommandPool());

        writeDescriptorForEachFrame([&bloomDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            bloomDesc.queue(queue, frameIndex, {}, {DescBloom::BINDING_UBO_BLOOM});
        });
    }

    void GPCBloom::pipelineInitialized() {
        //noop
    }

    void GPCBloom::windowResized() {
        using namespace SharedDescriptorTemplate;
        auto &bloomDesc = getDescriptor(SET_BLOOM_CANVAS);
        bloomDesc.get<vkh::CombinedMultiframeImageSampler>(0, BINDING_BLOOM_CANVAS_ORIGINAL)->setImageContext(
            engine.getRenderContextConfigurator<RCCFog>().getImageContext(
                RCCFog::RESULT_IMAGE_CONTEXT));
        bloomDesc.get<vkh::CombinedMultiframeImageSampler>(0, BINDING_BLOOM_CANVAS_BLURRED)->setImageContext(
            engine.getRenderContextConfigurator<RCCDownsampleForBlur>().getImageContext(
                RCCDownsampleForBlur::DST_IMAGE_CONTEXT)
        );

        writeDescriptorForEachFrame([&bloomDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            bloomDesc.queue(queue, frameIndex, {}, {BINDING_BLOOM_CANVAS_ORIGINAL, BINDING_BLOOM_CANVAS_BLURRED});
        });
    }


    void GPCBloom::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //no operation
    }

    void GPCBloom::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;

        vkh::SamplerRef sampler = pickFromRepository<vkh::SamplerRepo, vkh::SamplerRef>(
            VkSamplerCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .magFilter = VK_FILTER_LINEAR,
                .minFilter = VK_FILTER_LINEAR,
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

        descManager->appendCombinedMultiframeImgSampler(BINDING_BLOOM_CANVAS_ORIGINAL,
                                                        VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        vkh::factory::create<vkh::CombinedMultiframeImageSampler>(
                                                            engine.getCore(), sampler));
        descManager->appendCombinedMultiframeImgSampler(BINDING_BLOOM_CANVAS_BLURRED,
                                                        VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        vkh::factory::create<vkh::CombinedMultiframeImageSampler>(
                                                            engine.getCore(), sampler));
        appendUniqueDescriptor(SET_BLOOM_CANVAS, descriptors, std::move(descManager));
        appendDescriptor<DescBloom>(SET_BLOOM, descriptors);
        appendDescriptor<DescResolution>(SET_RESOLUTION, descriptors);
    }
}
