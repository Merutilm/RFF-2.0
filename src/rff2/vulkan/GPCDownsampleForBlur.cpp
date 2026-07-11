//
// Created by Merutilm on 2025-08-27.
//

#include "GPCDownsampleForBlur.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "SharedImageContextIndices.hpp"
#include "vulkan_helper/engine/repo/GlobalSamplerRepo.hpp"


namespace merutilm::rff2 {
    void GPCDownsampleForBlur::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
        // no operation
    }

    void GPCDownsampleForBlur::setRescaledResolution(const uint32_t descIndex, const glm::uvec2 &newResolution) const {
        auto &resDesc = getDescriptor(SET_RESAMPLE);
        auto &resUBO = resDesc.get<vkh::Uniform>(descIndex, BINDING_RESAMPLE_UBO);
        auto &resUBOHost = resUBO.getHostObject();
        resUBOHost.set<glm::uvec2>(TARGET_RESAMPLE_UBO_EXTENT, newResolution);
        resUBO.update();
    }

    void GPCDownsampleForBlur::pipelineInitialized() {
        writeDescriptorMF([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_RESAMPLE).queue(queue, frameIndex, {}, {BINDING_RESAMPLE_UBO});
        });
    }

    void GPCDownsampleForBlur::renderContextRefreshed() {
        auto &sic = wc.getSharedImageContext();
        auto &resampleDesc = getDescriptor(SET_RESAMPLE);

        resampleDesc.get<vkh::CombinedImageSampler>(DESC_INDEX_RESAMPLE_IMAGE_FOG, BINDING_RESAMPLE_SAMPLER)
                       .setImageContextMF(
                               sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY));
        resampleDesc.get<vkh::CombinedImageSampler>(DESC_INDEX_RESAMPLE_IMAGE_BLOOM, BINDING_RESAMPLE_SAMPLER)
                .setImageContextMF(
                        sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY));

        writeDescriptorMF([&resampleDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            resampleDesc.queue(queue, frameIndex, {}, {BINDING_RESAMPLE_SAMPLER});
        });
    }


    void GPCDownsampleForBlur::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }

    void GPCDownsampleForBlur::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        vkh::Sampler &sampler = pickFromGlobalRepository<vkh::GlobalSamplerRepo, vkh::Sampler &>(
                VkSamplerCreateInfo{.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
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
                                    .unnormalizedCoordinates = VK_FALSE});
        auto descManagers = std::vector<vkh::DescriptorManager>();
        descManagers.reserve(DESC_COUNT_RESAMPLE_IMAGE);
        for (uint32_t i = 0; i < DESC_COUNT_RESAMPLE_IMAGE; ++i) {
            auto descManager = vkh::DescriptorManager();
            auto combinedSampler = std::make_unique<vkh::CombinedImageSampler>(wc.core, sampler, true);
            descManager.appendCombinedImgSampler(BINDING_RESAMPLE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
                                                 std::move(combinedSampler));
            auto uboManager = vkh::HostDataObjectManager();
            uboManager.reserve<glm::uvec2>(TARGET_RESAMPLE_UBO_EXTENT);
            descManager.appendUBO(BINDING_RESAMPLE_UBO, VK_SHADER_STAGE_FRAGMENT_BIT,
                                  std::make_unique<vkh::Uniform>(wc.core, std::move(uboManager),
                                                                 vkh::BufferLock::LOCK_UNLOCK, false));
            descManagers.emplace_back(std::move(descManager));
        }
        appendUniqueDescriptor(SET_RESAMPLE, descriptors, std::move(descManagers));
    }
} // namespace merutilm::rff2
