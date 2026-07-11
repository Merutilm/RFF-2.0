//
// Created by Merutilm on 7/9/26.
//

#pragma once
#include "RenderPassAttachmentType.hpp"
#include "vulkan_helper/base/pch.hpp"

namespace merutilm::vkh {

    struct RenderPassAttachmentReference {
        const RenderPassAttachmentType attachmentType;
        const VkImageLayout layoutToUse;


        static RenderPassAttachmentReference none() {return {};}
    };


}
