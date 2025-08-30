//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include <vector>
#include "../impl/Swapchain.hpp"
#include "../struct/ImageInitInfo.hpp"
#include "../util/BufferImageUtils.hpp"
#include "../util/SwapchainUtils.hpp"

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

        static ImageContext createContext(CoreRef core, const ImageInitInfo &imageInitInfo) {
            ImageContext result = {};
            BufferImageUtils::initImage(core, imageInitInfo, &result.image, &result.imageMemory,
                                        &result.imageView, &result.mipmappedImageView);
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
            vkDestroyImageView(device, imgCtx.imageView, nullptr);
            if (imgCtx.mipmappedImageView != imgCtx.imageView) {
                vkDestroyImageView(device, imgCtx.mipmappedImageView, nullptr);
            }
            vkDestroyImage(device, imgCtx.image, nullptr);
            vkFreeMemory(device, imgCtx.imageMemory, nullptr);
        }

        static void destroyContext(CoreRef core, const MultiframeImageContext & imgCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            for (const auto &[image, imageFormat, imageMemory, imageView, mipmappedImageView, extent]: imgCtx) {
                vkDestroyImageView(device, imageView, nullptr);
                if (mipmappedImageView != imageView) {
                    vkDestroyImageView(device, mipmappedImageView, nullptr);
                }
                vkDestroyImage(device, image, nullptr);
                vkFreeMemory(device, imageMemory, nullptr);
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
                result[i].imageFormat = SwapchainUtils::SWAPCHAIN_IMAGE_FORMAT,
                result[i].imageMemory = VK_NULL_HANDLE;
                result[i].imageView = imageViews[i];
                result[i].mipmappedImageView = imageViews[i];
                result[i].extent = extent;
            }
            return result;
        }
    };
}
