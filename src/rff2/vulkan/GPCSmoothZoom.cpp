//
// Created by Merutilm on 7/28/26.
//

#include "GPCSmoothZoom.hpp"

#include "SharedImageContextIndices.hpp"
#include "desc/SharedDescriptorTemplate.hpp"
#include "vulkan-helper/engine/repo/GlobalSamplerRepo.hpp"

namespace merutilm::rff2 {


    void GPCSmoothZoom::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        // noop
    }

    void GPCSmoothZoom::pipelineInitialized() {
        // noop
    }


    void GPCSmoothZoom::renderContextRefreshed() {
        auto &sic = wc.getSharedImageContext();
        auto &resampleDesc = getDescriptor(SET_SAMPLE);
        resampleDesc.get<vkh::CombinedImageSampler>(0, BINDING_SAMPLE_SAMPLER)
                .setImageContextMF(sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_SECONDARY));


        writeDescriptorMF([&resampleDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            resampleDesc.queue(queue, frameIndex, {}, {BINDING_SAMPLE_SAMPLER});
        });
    }


    void GPCSmoothZoom::configurePushConstant(vkh::PipelineLayoutManager &pipelineLayoutManager) {
        // noop
    }


    void GPCSmoothZoom::configureDescriptors(std::vector<vkh::Descriptor *> &descriptors) {
        using namespace SharedDescriptorTemplate;
        vkh::Sampler &sampler = pickFromGlobalRepository<vkh::GlobalSamplerRepo, vkh::Sampler &>(
                VkSamplerCreateInfo{.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                    .pNext = nullptr,
                                    .flags = 0,
                                    .magFilter = VK_FILTER_LINEAR,
                                    .minFilter = VK_FILTER_LINEAR,
                                    .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
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
                                    .unnormalizedCoordinates = VK_FALSE});
        auto descManager = vkh::DescriptorManager();
        auto combinedSampler = std::make_unique<vkh::CombinedImageSampler>(wc.core, sampler, true);
        descManager.appendCombinedImgSampler(BINDING_SAMPLE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,
                                             std::move(combinedSampler));

        appendUniqueDescriptor(SET_SAMPLE, descriptors, std::move(descManager));
        appendDescriptor<DescSmoothZoom>(SET_SMOOTH_ZOOM, descriptors);
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescIterationSnapshotVariant>(SET_ITERATION_SNAPSHOT, descriptors);
    }
} // namespace merutilm::rff2
