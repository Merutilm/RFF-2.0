//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include <vector>
#include "../impl/Swapchain.hpp"
#include "../struct/ImageInitInfo.hpp"
#include "../util/BufferImageUtils.hpp"

namespace merutilm::vkh {
    struct ImageContext;

    using MultiframeImageContext = std::vector<ImageContext>;

    struct ImageContext {
        VkImage image;
        VkDeviceMemory imageMemory;
        VkImageView imageView;
        VkImageLayout imageLayout;

        static ImageContext createContext(CoreRef core, const ImageInitInfo &imageInitInfo) {
            ImageContext result = {};
            BufferImageUtils::initImage(core, imageInitInfo, &result.image, &result.imageMemory,
                                        &result.imageView);
            return result;
        }

        static MultiframeImageContext createMultiframeContext(CoreRef core, const ImageInitInfo &imageInitInfo) {
            const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
            std::vector<ImageContext> result(maxFramesInFlight);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                BufferImageUtils::initImage(core, imageInitInfo, &result[i].image, &result[i].imageMemory,
                                            &result[i].imageView);
            }
            return result;
        }

        static void destroyContext(CoreRef core, const ImageContext *const imgCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            vkDestroyImageView(device, imgCtx->imageView, nullptr);
            vkDestroyImage(device, imgCtx->image, nullptr);
            vkFreeMemory(device, imgCtx->imageMemory, nullptr);
        }

        static void destroyContext(CoreRef core, const MultiframeImageContext *const imgCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            for (const auto &[image, imageMemory, imageView, imageLayout]: *imgCtx) {
                vkDestroyImageView(device, imageView, nullptr);
                vkDestroyImage(device, image, nullptr);
                vkFreeMemory(device, imageMemory, nullptr);
            }
        }

        static MultiframeImageContext fromSwapchain(CoreRef core, SwapchainRef swapchain) {
            const auto images = swapchain.getSwapchainImages();
            const auto imageViews = swapchain.getSwapchainImageViews();
            const auto maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
            std::vector<ImageContext> result(maxFramesInFlight);
            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                result[i].image = images[i];
                result[i].imageMemory = VK_NULL_HANDLE;
                result[i].imageView = imageViews[i];
                result[i].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            }
            return result;
        }
    };
}
