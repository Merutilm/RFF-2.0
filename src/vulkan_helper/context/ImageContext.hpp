//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include <vector>
#include "../def/Swapchain.hpp"
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

        static MultiframeImageContext init(const Core &core, const ImageInitInfo &imageInitInfo) {
            const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
            std::vector<ImageContext> result(maxFramesInFlight);

            for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
                BufferImageUtils::initImage(core, imageInitInfo, &result[i].image, &result[i].imageMemory,
                                            &result[i].imageView);
            }
            return result;
        }

        static void destroy(const Core &core, const MultiframeImageContext &imgCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            for (const auto &[image, imageMemory, imageView, imageLayout]: imgCtx) {
                vkDestroyImageView(device, imageView, nullptr);
                vkDestroyImage(device, image, nullptr);
                vkFreeMemory(device, imageMemory, nullptr);
            }
        }

        static MultiframeImageContext fromSwapchain(const Swapchain &swapchain, const uint32_t maxFramesInFlight) {
            const auto images = swapchain.getSwapchainImages();
            const auto imageViews = swapchain.getSwapchainImageViews();
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
