//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/context/ImageContext.hpp>
namespace merutilm::vkh {
    ImageContext ImageContext::createContext(Core & core, const ImageInitInfo &imageInitInfo) {
        ImageContext result = {};
        BufferImageUtils::initImage(core, imageInitInfo, &result.image, &result.imageMemory,
                                    &result.imageView, &result.mipmappedImageView, &result.capacity);
        result.imageFormat = imageInitInfo.imageFormat;
        result.extent = {imageInitInfo.extent.width, imageInitInfo.extent.height};
        return result;
    }

    MultiframeImageContext ImageContext::createMultiframeContext(Core & core, const ImageInitInfo &imageInitInfo) {
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        std::vector<ImageContext> result(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            result[i] = createContext(core, imageInitInfo);
        }

        return result;
    }

    void ImageContext::destroyContext(Core & core, const ImageContext &imgCtx) {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        vkDestroyImageView(device, imgCtx.imageView, nullptr);
        if (imgCtx.mipmappedImageView != imgCtx.imageView) {
            vkDestroyImageView(device, imgCtx.mipmappedImageView, nullptr);
        }
        vkDestroyImage(device, imgCtx.image, nullptr);
        vkFreeMemory(device, imgCtx.imageMemory, nullptr);
    }

    void ImageContext::destroyContext(Core & core, const MultiframeImageContext &imgCtx) {
        for (const auto &ctx: imgCtx) {
            destroyContext(core, ctx);
        }
    }

    MultiframeImageContext ImageContext::fromSwapchain(Core & core, const Swapchain & swapchain) {
        const auto images = swapchain.getSwapchainImages();
        const auto imageViews = swapchain.getSwapchainImageViews();
        const auto maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        const auto extent = swapchain.getSwapchainExtent();

        std::vector<ImageContext> result(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            result[i].image = images[i];
            result[i].imageFormat = core.getPhysicalDeviceLoader().getPrimarySurfaceFormat(),
                    result[i].imageMemory = VK_NULL_HANDLE;
            result[i].imageView = imageViews[i];
            result[i].mipmappedImageView = imageViews[i];
            result[i].extent = extent;
            result[i].capacity = 0;
        }
        return result;
    }
}
