//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include <vulkan_helper/engine/context/ImageContext.hpp>

namespace merutilm::vkh {
    struct RenderPassAttachment {
        VkAttachmentDescription attachment;
        MultiframeImageContext imageContext;
    };
}
