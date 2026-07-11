//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include <vulkan_helper/engine/window/Swapchain.hpp>
#include <vulkan_helper/engine/wrapped/ImageInitInfo.hpp>
#include <vulkan_helper/util/BufferImageUtils.hpp>

namespace merutilm::vkh {
    struct ImageContext;

    using MultiframeImageContext = std::vector<ImageContext>;

    struct ImageContext {
        VkImage image = VK_NULL_HANDLE;
        VkFormat imageFormat = VK_FORMAT_UNDEFINED;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkImageView mipmappedImageView = VK_NULL_HANDLE;
        VkExtent2D extent = {};
        VkDeviceSize capacity = 0;

        static ImageContext createContext(Core & core, const ImageInitInfo &imageInitInfo);

        static MultiframeImageContext createMultiframeContext(Core & core, const ImageInitInfo &imageInitInfo);

        static void destroyContext(Core & core, const ImageContext & imgCtx);

        static void destroyContext(Core & core, const MultiframeImageContext & imgCtx);

        static MultiframeImageContext fromSwapchain(Core & core, const Swapchain & swapchain);
    };
}
