//
// Created by Merutilm on 2025-09-09.
//

#include "GPCStaticImage2Map.hpp"

#include "SharedDescriptorTemplate.hpp"
#include "../../vulkan_helper/repo/GlobalSamplerRepo.hpp"
#include "../../vulkan_helper/util/BufferImageContextUtils.hpp"
#include "../io/RFFStaticMapBinary.h"

namespace merutilm::rff2 {
    void GPCStaticImage2Map::updateQueue(vkh::DescriptorUpdateQueue &queue, uint32_t frameIndex) {
        //noop
    }

    void GPCStaticImage2Map::pipelineInitialized() {
        //noop
    }

    void GPCStaticImage2Map::renderContextRefreshed() {
        //noop
    }

    void GPCStaticImage2Map::setImages(const cv::Mat &normal, const cv::Mat &zoomed) const {
        const auto n = vkh::BufferImageContextUtils::imageFromByteColorArray(wc.core, wc.getCommandPool(), VK_FORMAT_R16G16B16A16_UNORM, static_cast<uint32_t>(normal.cols), static_cast<uint32_t>(normal.rows),
        4, 16, false, reinterpret_cast<std::byte*>(normal.data));
        const auto z = vkh::BufferImageContextUtils::imageFromByteColorArray(wc.core, wc.getCommandPool(), VK_FORMAT_R16G16B16A16_UNORM, static_cast<uint32_t>(normal.cols), static_cast<uint32_t>(normal.rows),
        4, 16, false, reinterpret_cast<std::byte*>(zoomed.data));
        auto &imageDesc = getDescriptor(SET_IMAGES);
        imageDesc.get<vkh::CombinedImageSampler>(0, BINDING_IMAGES_NORMAL)->setUniqueImageContext(n);
        imageDesc.get<vkh::CombinedImageSampler>(0, BINDING_IMAGES_ZOOMED)->setUniqueImageContext(z);
        writeDescriptorMF([&imageDesc](vkh::DescriptorUpdateQueue &queue, const uint32_t frameIndex) {
            imageDesc.queue(queue, frameIndex, {}, {BINDING_IMAGES_NORMAL, BINDING_IMAGES_ZOOMED});
        });
    }

    void GPCStaticImage2Map::configurePushConstant(vkh::PipelineLayoutManagerRef pipelineLayoutManager) {
        //noop
    }

    void GPCStaticImage2Map::configureDescriptors(std::vector<vkh::DescriptorPtr> &descriptors) {
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

        descManager->appendCombinedImgSampler(BINDING_IMAGES_NORMAL,
                                              VK_SHADER_STAGE_FRAGMENT_BIT,
                                              vkh::factory::create<vkh::CombinedImageSampler>(
                                                  wc.core, sampler, false));
        descManager->appendCombinedImgSampler(BINDING_IMAGES_ZOOMED,
                                              VK_SHADER_STAGE_FRAGMENT_BIT,
                                              vkh::factory::create<vkh::CombinedImageSampler>(
                                                  wc.core, sampler, false));
        appendUniqueDescriptor(SET_IMAGES, descriptors, std::move(descManager));
        appendDescriptor<SharedDescriptorTemplate::DescVideo>(SET_VIDEO, descriptors);
    }
}
