//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include "../context/ImageContext.hpp"

namespace merutilm::vkh {
    struct RenderPassAttachment {
        VkAttachmentDescription attachment;
        MultiframeImageContext imageContext;
    };
}
