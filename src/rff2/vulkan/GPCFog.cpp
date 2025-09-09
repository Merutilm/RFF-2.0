//
// Created by Merutilm on 2025-08-15.
//

#include "GPCFog.hpp"

#include "RCCDownsampleForBlur.hpp"
#include "RCC1.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/repo/GlobalSamplerRepo.hpp"
#include "../constants/VulkanWindowConstants.hpp"

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
        fogUBO.update();
    }

    void GPCFog::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorMF([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_FOG).queue(queue, frameIndex, {}, {DescFog::BINDING_UBO_FOG});
        });
    }

    void GPCFog::renderContextRefreshed() {
        auto &sic = wc.getSharedImageContext();
        auto &fogDesc = getDescriptor(SET_FOG_CANVAS);

        switch (wc.getAttachmentIndex()) {
            case Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX: {
                fogDesc.get<vkh::CombinedImageSampler>(0, BINDING_FOG_CANVAS_ORIGINAL)->setImageContextMF(
                          sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY));
                fogDesc.get<vkh::CombinedImageSampler>(0, BINDING_FOG_CANVAS_BLURRED)->setImageContextMF(
                    sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_SECONDARY));

                break;
            }
            case Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX: {
                fogDesc.get<vkh::CombinedImageSampler>(0, BINDING_FOG_CANVAS_ORIGINAL)->setImageContextMF(
                         sic.getImageContextMF(SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_PRIMARY));
                fogDesc.get<vkh::CombinedImageSampler>(0, BINDING_FOG_CANVAS_BLURRED)->setImageContextMF(
                    sic.getImageContextMF(SharedImageContextIndices::MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_SECONDARY));

                break;
            }
            default: {
                //noop
            }
        }


        writeDescriptorMF([&fogDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            fogDesc.queue(queue, frameIndex, {}, {BINDING_FOG_CANVAS_ORIGINAL, BINDING_FOG_CANVAS_BLURRED});
        });
    }


    void GPCFog::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //no operation
    }

    void GPCFog::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;

        vkh::SamplerRef sampler = pickFromGlobalRepository<vkh::GlobalSamplerRepo, vkh::SamplerRef>(
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

        descManager->appendCombinedImgSampler(BINDING_FOG_CANVAS_ORIGINAL,
                                                        VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        vkh::factory::create<vkh::CombinedImageSampler>(
                                                            wc.core, sampler, true));
        descManager->appendCombinedImgSampler(BINDING_FOG_CANVAS_BLURRED,
                                                        VK_SHADER_STAGE_FRAGMENT_BIT,
                                                        vkh::factory::create<vkh::CombinedImageSampler>(
                                                            wc.core, sampler, true));
        appendUniqueDescriptor(SET_FOG_CANVAS, descriptors, std::move(descManager));
        appendDescriptor<DescFog>(SET_FOG, descriptors);
    }
}
