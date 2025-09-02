//
// Created by Merutilm on 2025-08-15.
//

#include "GPCFog.hpp"

#include "RCCDownsampleForBlur.hpp"
#include "RCC1.hpp"
#include "SharedDescriptorTemplate.hpp"

namespace merutilm::rff2 {
    void GPCFog::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        //no operation
    }


    void GPCFog::setFog(const ShdFogAttribute &fog) const {
        using namespace SharedDescriptorTemplate;
        auto &fogDesc = getDescriptor(SET_FOG);
        const auto &fogUBO = *fogDesc.get<vkh::Uniform>(0, DescFog::BINDING_UBO_FOG);
        auto &fogUBOHost = fogUBO.getHostObject();
        fogUBOHost.set<float>(DescFog::TARGET_FOG_RADIUS, fog.radius);
        fogUBOHost.set<float>(DescFog::TARGET_FOG_OPACITY, fog.opacity);

        updateBufferForEachFrame([&fogUBO](const uint32_t frameIndex) {
            fogUBO.update(frameIndex);
        });
    }

    void GPCFog::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorForEachFrame([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_FOG).queue(queue, frameIndex, {}, {DescFog::BINDING_UBO_FOG});
        });
    }

    void GPCFog::windowResized() {
        using namespace SharedDescriptorTemplate;
        auto &fogDesc = getDescriptor(SET_FOG_CANVAS);
        fogDesc.get<vkh::CombinedMultiframeImageSampler>(0, BINDING_FOG_CANVAS_ORIGINAL)->setImageContext(
           engine.getSharedImageContext().getMultiframeContext(SharedImageContextIndices::MF_RENDER_IMAGE_PRIMARY));
        fogDesc.get<vkh::CombinedMultiframeImageSampler>(0, BINDING_FOG_CANVAS_BLURRED)->setImageContext(
            engine.getSharedImageContext().getMultiframeContext(SharedImageContextIndices::MF_RENDER_DOWNSAMPLED_IMAGE_SECONDARY));

        writeDescriptorForEachFrame([&fogDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            fogDesc.queue(queue, frameIndex, {}, {BINDING_FOG_CANVAS_ORIGINAL, BINDING_FOG_CANVAS_BLURRED});
        });
    }


    void GPCFog::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //no operation
    }

    void GPCFog::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
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

        descManager->appendCombinedMultiframeImgSampler(BINDING_FOG_CANVAS_ORIGINAL,
                                                        VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        vkh::factory::create<vkh::CombinedMultiframeImageSampler>(
                                                            engine.getCore(), sampler));
        descManager->appendCombinedMultiframeImgSampler(BINDING_FOG_CANVAS_BLURRED,
                                                        VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        vkh::factory::create<vkh::CombinedMultiframeImageSampler>(
                                                            engine.getCore(), sampler));
        appendUniqueDescriptor(SET_FOG_CANVAS, descriptors, std::move(descManager));
        appendDescriptor<DescFog>(SET_FOG, descriptors);
        appendDescriptor<DescResolution>(SET_RESOLUTION, descriptors);
    }
}
