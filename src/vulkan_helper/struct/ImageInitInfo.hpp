//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include <vulkan/vulkan.h>

namespace merutilm::vkh {
    struct ImageInitInfo {
        VkImageType imageType;
        VkImageViewType imageViewType;
        VkFormat imageFormat;
        VkExtent3D extent;
        uint32_t mipLevels;
        uint32_t arrayLayers;
        VkSampleCountFlagBits samples;
        VkImageTiling imageTiling;
        VkImageUsageFlags usage;
        VkImageLayout initialLayout;
        VkMemoryPropertyFlags properties;
    };

    
}
