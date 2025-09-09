//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include "../core/vkh_core.hpp"
#include "../impl/Swapchain.hpp"
#include "../struct/ImageInitInfo.hpp"
#include "../util/BufferImageUtils.hpp"

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

        static ImageContext createContext(CoreRef core, const ImageInitInfo &imageInitInfo) {
            ImageContext result = {};
            BufferImageUtils::initImage(core, imageInitInfo, &result.image, &result.imageMemory,
                                        &result.imageView, &result.mipmappedImageView, &result.capacity);
            result.imageFormat = imageInitInfo.imageFormat;
            result.extent = {imageInitInfo.extent.width, imageInitInfo.extent.height};
            return result;
        }

        static MultiframeImageContext createMultiframeContext(CoreRef core, const ImageInitInfo &imageInitInfo) {
            const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
            std::vector<ImageContext> result(maxFramesInFlight);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                result[i] = createContext(core, imageInitInfo);
            }

            return result;
        }

        static void destroyContext(CoreRef core, const ImageContext & imgCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            allocator::invoke(vkDestroyImageView, device, imgCtx.imageView, nullptr);
            if (imgCtx.mipmappedImageView != imgCtx.imageView) {
                allocator::invoke(vkDestroyImageView, device, imgCtx.mipmappedImageView, nullptr);
            }
            allocator::invoke(vkDestroyImage, device, imgCtx.image, nullptr);
            allocator::invoke(vkFreeMemory, device, imgCtx.imageMemory, nullptr);
        }

        static void destroyContext(CoreRef core, const MultiframeImageContext & imgCtx) {
            for (const auto &ctx: imgCtx) {
                destroyContext(core, ctx);
            }
        }

        static MultiframeImageContext fromSwapchain(CoreRef core, SwapchainRef swapchain) {
            const auto images = swapchain.getSwapchainImages();
            const auto imageViews = swapchain.getSwapchainImageViews();
            const auto maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
            const auto extent = swapchain.populateSwapchainExtent();

            std::vector<ImageContext> result(maxFramesInFlight);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                result[i].image = images[i];
                result[i].imageFormat = config::SWAPCHAIN_IMAGE_FORMAT,
                result[i].imageMemory = VK_NULL_HANDLE;
                result[i].imageView = imageViews[i];
                result[i].mipmappedImageView = imageViews[i];
                result[i].extent = extent;
                result[i].capacity = 0;
            }
            return result;
        }
    };
}
