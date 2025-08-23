//
// Created by Merutilm on 2025-07-09.
//

#include "Swapchain.hpp"

#include "../exception/exception.hpp"
#include "../util/BufferImageUtils.hpp"

namespace merutilm::vkh {
    SwapchainImpl::SwapchainImpl(SurfaceRef surface, PhysicalDeviceLoaderRef physicalDevice,
                         LogicalDeviceRef logicalDevice) : surface(surface), physicalDevice(physicalDevice),
                                                               logicalDevice(logicalDevice) {
        SwapchainImpl::init();
    }

    SwapchainImpl::~SwapchainImpl() {
        SwapchainImpl::destroy();
    }

    void SwapchainImpl::matchViewportAndScissor(const VkCommandBuffer cbh) const {
        const auto [width, height] = populateSwapchainExtent();
        const VkViewport viewport = {
            .x = 0,
            .y = 0,
            .width = static_cast<float>(width),
            .height = static_cast<float>(height),
            .minDepth = 0,
            .maxDepth = 1
        };
        const VkRect2D scissor = {
            .offset = {0, 0},
            .extent = {width, height}
        };


        vkCmdSetViewport(cbh, 0, 1, &viewport);
        vkCmdSetScissor(cbh, 0, 1, &scissor);
    }


    void SwapchainImpl::recreate() {
        destroyImageViews();
        oldSwapchain = swapchain;
        createSwapchain(&swapchain, oldSwapchain);
        vkDestroySwapchainKHR(logicalDevice.getLogicalDeviceHandle(), oldSwapchain, nullptr);
        setupSwapchainImages();
    }


    VkExtent2D SwapchainImpl::populateSwapchainExtent() const {
        const HWND window = surface.getTargetWindow().getWindowHandle();
        const auto capabilities = physicalDevice.populateSurfaceCapabilities();

        if (capabilities.currentExtent.width == UINT32_MAX) {
            return capabilities.currentExtent;
        }

        RECT rect;
        GetClientRect(window, &rect);
        const VkExtent2D extent = {
            .width = std::clamp(static_cast<uint32_t>(rect.right - rect.left), capabilities.minImageExtent.width,
                                capabilities.maxImageExtent.width),
            .height = std::clamp(static_cast<uint32_t>(rect.bottom - rect.top), capabilities.minImageExtent.height,
                                 capabilities.maxImageExtent.height),

        };
        return extent;
    }


    void SwapchainImpl::init() {
        createSwapchain(&swapchain, nullptr);
        setupSwapchainImages();
    }

    void SwapchainImpl::createSwapchain(VkSwapchainKHR *target, const VkSwapchainKHR old) const {
        const uint32_t maxFramesInFlight = physicalDevice.getMaxFramesInFlight();
        const auto &[graphicsFamily, presentFamily] = physicalDevice.getQueueFamilyIndices();
        std::array queueFamilyIndices = {graphicsFamily.value(), presentFamily.value()};

        const VkSurfaceCapabilitiesKHR capabilities = physicalDevice.populateSurfaceCapabilities();

        if (const VkSwapchainCreateInfoKHR createInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = surface.getSurfaceHandle(),
            .minImageCount = maxFramesInFlight,
            .imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
            .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = populateSwapchainExtent(),
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = graphicsFamily == presentFamily
                                    ? VK_SHARING_MODE_EXCLUSIVE
                                    : VK_SHARING_MODE_CONCURRENT,
            .queueFamilyIndexCount = graphicsFamily == presentFamily
                                         ? 0
                                         : static_cast<uint32_t>(queueFamilyIndices.size()),
            .pQueueFamilyIndices = graphicsFamily == presentFamily ? nullptr : queueFamilyIndices.data(),
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
            .clipped = VK_TRUE,
            .oldSwapchain = old
        }; vkCreateSwapchainKHR(logicalDevice.getLogicalDeviceHandle(), &createInfo, nullptr, target) !=
           VK_SUCCESS) {
            throw exception_init("Failed to create swapchain!");
        }
    }

    void SwapchainImpl::setupSwapchainImages() {
        uint32_t maxFramesInFlight = physicalDevice.getMaxFramesInFlight();
        swapchainImages.resize(maxFramesInFlight);
        swapchainImageViews.resize(maxFramesInFlight);

        vkGetSwapchainImagesKHR(logicalDevice.getLogicalDeviceHandle(), swapchain, &maxFramesInFlight,
                                swapchainImages.data());
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            BufferImageUtils::createImageView(logicalDevice.getLogicalDeviceHandle(), swapchainImages[i],
                                         VK_IMAGE_VIEW_TYPE_2D, 1, VK_FORMAT_R8G8B8A8_SRGB, &swapchainImageViews[i]);
        }
    }


    void SwapchainImpl::destroy() {
        destroyImageViews();
        vkDestroySwapchainKHR(logicalDevice.getLogicalDeviceHandle(), swapchain, nullptr);
    }

    void SwapchainImpl::destroyImageViews() const {
        const uint32_t maxFramesInFlight = physicalDevice.getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            vkDestroyImageView(logicalDevice.getLogicalDeviceHandle(), swapchainImageViews[i], nullptr);
        }
    }
}
