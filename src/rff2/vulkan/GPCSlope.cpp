//
// Created by Merutilm on 2025-08-15.
//

#include "GPCSlope.hpp"

#include "RCC1.hpp"
#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/repo/GlobalSamplerRepo.hpp"
#include "../constants/VulkanWindowConstants.hpp"

namespace merutilm::rff2 {
    void GPCSlope::updateQueue(vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {

        //no operation
    }


    void GPCSlope::setSlope(const ShdSlopeAttribute &slope) const {
        using namespace SharedDescriptorTemplate;
        auto &slopeDesc = getDescriptor(SET_SLOPE);
        const auto &slopeUBO = slopeDesc.get<vkh::Uniform>(0, DescSlope::BINDING_UBO_SLOPE);
        auto &slopeUBOHost = slopeUBO->getHostObject();
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_DEPTH, slope.depth);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_REFLECTION_RATIO, slope.reflectionRatio);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_OPACITY, slope.opacity);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_ZENITH, slope.zenith);
        slopeUBOHost.set<float>(DescSlope::TARGET_SLOPE_AZIMUTH, slope.azimuth);
        slopeUBO->update();
    }

    void GPCSlope::pipelineInitialized() {
        using namespace SharedDescriptorTemplate;
        writeDescriptorMF([this](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            getDescriptor(SET_SLOPE).queue(queue, frameIndex, {}, {DescSlope::BINDING_UBO_SLOPE});
        });
    }

    void GPCSlope::renderContextRefreshed() {
        auto &sic = wc.getSharedImageContext();
        auto &inputDesc = getDescriptor(SET_PREV_RESULT);

        switch (wc.getAttachmentIndex()) {
            case Constants::VulkanWindow::MAIN_WINDOW_ATTACHMENT_INDEX: {
                const auto &input = sic.getImageContextMF(SharedImageContextIndices::MF_MAIN_RENDER_IMAGE_PRIMARY);
                inputDesc.get<vkh::CombinedImageSampler>(0, BINDING_PREV_RESULT_SAMPLER)->setImageContextMF(input);
                break;
            }
            case Constants::VulkanWindow::VIDEO_WINDOW_ATTACHMENT_INDEX: {
                const auto &input = sic.getImageContextMF(SharedImageContextIndices::MF_VIDEO_RENDER_IMAGE_PRIMARY);
                inputDesc.get<vkh::CombinedImageSampler>(0, BINDING_PREV_RESULT_SAMPLER)->setImageContextMF(input);
                break;
            }
            default: {
                //noop
            }
        }

        writeDescriptorMF([&inputDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            inputDesc.queue(queue, frameIndex, {}, {BINDING_PREV_RESULT_SAMPLER});
        });

    }

    void GPCSlope::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void GPCSlope::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
        using namespace SharedDescriptorTemplate;
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
        descManager->appendCombinedImgSampler(BINDING_PREV_RESULT_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, vkh::factory::create<vkh::CombinedImageSampler>(wc.core, sampler, true));

        appendUniqueDescriptor(SET_PREV_RESULT, descriptors, std::move(descManager));
        appendDescriptor<DescIteration>(SET_ITERATION, descriptors);
        appendDescriptor<DescSlope>(SET_SLOPE, descriptors);
    }
}
