//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include <vulkan/vulkan.h>

#include <vector>

namespace merutilm::mvk {
    struct RenderPassAttachment {
        VkAttachmentDescription attachment;
        MultiframeImageContext imageContext;
    };
}
