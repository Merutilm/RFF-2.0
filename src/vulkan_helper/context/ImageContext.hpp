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
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
        VkImageView writeImageView = VK_NULL_HANDLE;
        VkImageView readImageView = VK_NULL_HANDLE;
        std::vector<VkImageLayout> mipImageLayout = {};
        VkExtent2D extent = {};

        static ImageContext createContext(CoreRef core, const ImageInitInfo &imageInitInfo) {
            ImageContext result = {};
            BufferImageUtils::initImage(core, imageInitInfo, &result.image, &result.imageMemory,
                                        &result.writeImageView, &result.readImageView);
            result.extent = {imageInitInfo.extent.width, imageInitInfo.extent.height};
            const auto mipLevels = BufferImageUtils::genMipLevels(imageInitInfo);
            result.mipImageLayout = std::vector<VkImageLayout>(mipLevels);
            std::ranges::fill(result.mipImageLayout, VK_IMAGE_LAYOUT_UNDEFINED);
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

        static void destroyContext(CoreRef core, const ImageContext *const imgCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            vkDestroyImageView(device, imgCtx->writeImageView, nullptr);
            vkDestroyImageView(device, imgCtx->readImageView, nullptr);
            vkDestroyImage(device, imgCtx->image, nullptr);
            vkFreeMemory(device, imgCtx->imageMemory, nullptr);
        }

        static void destroyContext(CoreRef core, const MultiframeImageContext *const imgCtx) {
            const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
            for (const auto &[image, imageMemory, writeImageView, readImageView, imageLayout, extent]: *imgCtx) {
                vkDestroyImageView(device, writeImageView, nullptr);
                vkDestroyImageView(device, readImageView, nullptr);
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
                result[i].imageMemory = VK_NULL_HANDLE;
                result[i].writeImageView = imageViews[i];
                result[i].readImageView = imageViews[i];
                result[i].extent = extent;
                result[i].mipImageLayout = {VK_IMAGE_LAYOUT_UNDEFINED};
            }
            return result;
        }
    };
}
