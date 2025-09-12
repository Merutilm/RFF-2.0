//
// Created by Merutilm on 2025-09-10.
//

#include "CPCImageRGBA2BGR.hpp"

#include "SharedImageContextIndices.hpp"
#include "../../vulkan_helper/repo/GlobalSamplerRepo.hpp"


namespace merutilm::rff2 {
    void CPCImageRGBA2BGR::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        //noop
    }

    void CPCImageRGBA2BGR::pipelineInitialized() {
        //noop
    }

    void CPCImageRGBA2BGR::renderContextRefreshed() {
        using namespace SharedImageContextIndices;
        auto &desc = getDescriptor(SET_INFO);
        auto &prevImg = *desc.get<vkh::CombinedImageSampler>(0, BINDING_PREV_IMAGE_SAMPLER);
        prevImg.setImageContextMF(wc.getSharedImageContext().getImageContextMF(MF_VIDEO_RENDER_IMAGE_SECONDARY));
        const auto &extent = prevImg.getImageContextMF()[0].extent;
        auto &ssbo = *desc.get<vkh::ShaderStorage>(0, BINDING_OUTPUT_SSBO);
        ssbo.getHostObject().resizeAndClear<uint32_t>(TARGET_OUTPUT_SSBO_DATA,
                                                      extent.width * extent.height * 3 / 4 + 1);
        ssbo.reloadBuffer();
        ssbo.lock(wc.getCommandPool());
        setExtent(extent);
        writeDescriptorMF(
            [&desc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
                desc.queue(queue, frameIndex, {}, {BINDING_PREV_IMAGE_SAMPLER, BINDING_OUTPUT_SSBO});
            });
    }

    const vkh::BufferContext &CPCImageRGBA2BGR::getBufferContext(const uint32_t frameIndex) const {
        using namespace SharedImageContextIndices;
        return getDescriptor(SET_INFO).get<vkh::ShaderStorage>(0, BINDING_OUTPUT_SSBO)->getBufferContextMF()[
            frameIndex];
    }

    void CPCImageRGBA2BGR::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void CPCImageRGBA2BGR::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        auto descManager = vkh::factory::create<vkh::DescriptorManager>();
        vkh::SamplerRef sampler = pickFromGlobalRepository<vkh::GlobalSamplerRepo, vkh::SamplerRef>(
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
        descManager->appendCombinedImgSampler(BINDING_PREV_IMAGE_SAMPLER, VK_SHADER_STAGE_COMPUTE_BIT, vkh::factory::create<vkh::CombinedImageSampler>(wc.core, sampler, true));

        auto hdm = vkh::factory::create<vkh::HostDataObjectManager>();
        hdm->reserveArray<uint32_t>(TARGET_OUTPUT_SSBO_DATA, 1);

        descManager->appendSSBO(BINDING_OUTPUT_SSBO, VK_SHADER_STAGE_COMPUTE_BIT,
                                vkh::factory::create<vkh::ShaderStorage>(
                                    wc.core, std::move(hdm), vkh::BufferLock::LOCK_UNLOCK, true));

        appendUniqueDescriptor(SET_INFO, descriptors, std::move(descManager));
    }
}
