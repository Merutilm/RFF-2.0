//
// Created by Merutilm on 2025-08-29.
//

#pragma once
#include <vulkan/vulkan.h>

namespace merutilm::vkh {
    struct SwapchainUtils {

        static constexpr VkFormat SWAPCHAIN_IMAGE_FORMAT = VK_FORMAT_R8G8B8A8_UNORM;

        explicit SwapchainUtils() = delete;
    };
}
