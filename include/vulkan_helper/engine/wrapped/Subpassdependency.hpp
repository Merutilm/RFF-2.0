//
// Created by Merutilm on 7/9/26.
//

#pragma once
#include "vulkan_helper/base/pch.hpp"
namespace merutilm::vkh {
    struct SubpassDependency {
        const VkPipelineStageFlags srcPipelineStageFlags;
        const VkPipelineStageFlags dstPipelineStageFlags;
        const VkAccessFlags srcAccessFlags;
        const VkAccessFlags dstAccessFlags;
        const VkDependencyFlags dependencyFlags;


        static SubpassDependency none() {return {};}
    };
}
