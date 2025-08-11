//
// Created by Merutilm on 2025-08-01.
//

#pragma once
#include <list>
#include <vulkan/vulkan.h>

namespace merutilm::vkh {
    struct DescriptorUpdateContext {
        VkWriteDescriptorSet writeSet;
        VkDescriptorBufferInfo bufferInfo;
        VkDescriptorImageInfo imageInfo;
    };

    using DescriptorUpdateQueue = std::list<DescriptorUpdateContext>;
}
