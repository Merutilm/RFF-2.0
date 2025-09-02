//
// Created by Merutilm on 2025-08-27.
//

#include "GPCResample.hpp"

#include "SharedDescriptorTemplate.hpp"



namespace merutilm::rff2 {
    void GPCResample::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        //no operation
    }

    void GPCResample::setTargetImageContext(const uint32_t descIndex, const vkh::MultiframeImageContext &context) const {
        auto &resultDesc = getDescriptor(SET_RESAMPLE);
        resultDesc.get<vkh::CombinedMultiframeImageSampler>(descIndex, BINDING_RESAMPLE_SAMPLER)->setImageContext(context);

        writeDescriptorForEachFrame([&resultDesc, &descIndex](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            resultDesc.queue(queue, frameIndex, {descIndex}, {BINDING_RESAMPLE_SAMPLER});
        });
    }

    void GPCResample::setRescaledResolution(const uint32_t descIndex, const glm::uvec2 &newResolution) const {
        auto &resDesc = getDescriptor(SET_RESAMPLE);
        auto &resUBO = *resDesc.get<vkh::Uniform>(descIndex, BINDING_RESAMPLE_UBO);
        auto &resUBOHost = resUBO.getHostObject();
        resUBOHost.set<glm::uvec2>(TARGET_RESAMPLE_EXTENT, newResolution);
        updateBufferForEachFrame([&resUBO](const uint32_t frameIndex) {
            resUBO.update(frameIndex);
        });
    }

    void GPCResample::pipelineInitialized() {
        writeDescriptorForEachFrame([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_RESAMPLE).queue(queue, frameIndex, {}, {BINDING_RESAMPLE_UBO});
        });
    }

    void GPCResample::windowResized() {
        //no operation
    }


    void GPCResample::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void GPCResample::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        vkh::SamplerRef sampler = pickFromRepository<vkh::SamplerRepo, vkh::SamplerRef>(
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
        auto descManagers = std::vector<vkh::DescriptorManager>(RESAMPLE_IMAGES_COUNT_PER_FRAME);
        for (uint32_t i = 0; i < RESAMPLE_IMAGES_COUNT_PER_FRAME; ++i) {
            auto descManager = vkh::factory::create<vkh::DescriptorManager>();
            auto combinedSampler = vkh::factory::create<vkh::CombinedMultiframeImageSampler>(engine.getCore(), sampler);
            descManager->appendCombinedMultiframeImgSampler(BINDING_RESAMPLE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
                                                            std::move(combinedSampler));

            auto uboManager = vkh::factory::create<vkh::HostDataObjectManager>();
            uboManager->reserve<glm::uvec2>(TARGET_RESAMPLE_EXTENT);
            descManager->appendUBO(BINDING_RESAMPLE_UBO, VK_SHADER_STAGE_FRAGMENT_BIT,
                                   vkh::factory::create<vkh::Uniform>(engine.getCore(), std::move(uboManager),
                                                                      vkh::BufferLock::LOCK_UNLOCK));
            descManagers[i] = std::move(descManager);
        }
        appendUniqueDescriptor(SET_RESAMPLE, descriptors, std::move(descManagers));
    }
}
