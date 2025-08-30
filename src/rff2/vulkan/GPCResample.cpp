//
// Created by Merutilm on 2025-08-27.
//

#include "GPCResample.hpp"

#include "SharedDescriptorTemplate.hpp"


namespace merutilm::rff2 {
    void GPCResample::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    void GPCResample::setTargetImageContext(const vkh::MultiframeImageContext &context,
                                                             const uint32_t frameIndex) const {
        const auto &resultDesc = getDescriptor(SET_RESAMPLE);
        auto &resultManager = resultDesc.getDescriptorManager();
        resultManager.get<vkh::CombinedMultiframeImageSampler>(BINDING_RESAMPLE_SAMPLER)->setImageContext(context);

        writeDescriptor([&resultDesc, &frameIndex](vkh::DescriptorUpdateQueue &queue) {
            resultDesc.queue(queue, frameIndex);
        });

    }

    void GPCResample::setResolution(const glm::uvec2 &newResolution) const {
        const auto &resDesc = getDescriptor(SET_RESAMPLE);
        auto &resManager = resDesc.getDescriptorManager();
        auto &resUBO = *resManager.get<vkh::Uniform>(BINDING_RESAMPLE_UBO);
        auto &resUBOHost = resUBO.getHostObject();
        resUBOHost.set<glm::uvec2>(TARGET_RESAMPLE_EXTENT, newResolution);
        updateBufferForEachFrame([&resUBO](const uint32_t frameIndex) {
            resUBO.update(frameIndex);
        });
    }

    void GPCResample::pipelineInitialized() {
        writeDescriptorForEachFrame([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_RESAMPLE).queue(queue, frameIndex, {BINDING_RESAMPLE_UBO});
        });
    }

    void GPCResample::windowResized() {
        //no operation
    }


    void GPCResample::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void GPCResample::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
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
        descManager->appendCombinedMultiframeImgSampler(BINDING_RESAMPLE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        std::move(combinedSampler));
        auto uboManager = vkh::Factory::create<vkh::HostBufferObjectManager>();
        uboManager->reserve<glm::uvec2>(TARGET_RESAMPLE_EXTENT);
        descManager->appendUBO(BINDING_RESAMPLE_UBO, VK_SHADER_STAGE_FRAGMENT_BIT,
                               vkh::Factory::create<vkh::Uniform>(engine.getCore(), std::move(uboManager),
                                                                  vkh::BufferLock::LOCK_UNLOCK));
        appendUniqueDescriptor(SET_RESAMPLE, descriptors, std::move(descManager));
    }
}
