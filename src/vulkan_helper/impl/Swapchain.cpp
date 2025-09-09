//
// Created by Merutilm on 2025-07-09.
//

#include "Swapchain.hpp"

#include "../core/vkh_core.hpp"
#include "../util/BufferImageUtils.hpp"

namespace merutilm::vkh {
    SwapchainImpl::SwapchainImpl(CoreRef core, SurfaceRef surface) : CoreHandler(core), surface(surface) {
        SwapchainImpl::init();
    }

    SwapchainImpl::~SwapchainImpl() {
        SwapchainImpl::destroy();
    }


    void SwapchainImpl::recreate() {
        destroyImageViews();
        oldSwapchain = swapchain;
        createSwapchain(&swapchain, oldSwapchain);
        vkDestroySwapchainKHR(core.getLogicalDevice().getLogicalDeviceHandle(), oldSwapchain, nullptr);
        setupSwapchainImages();
    }


    VkExtent2D SwapchainImpl::populateSwapchainExtent() const {
        const HWND window = surface.getTargetWindow().getWindowHandle();
        const auto capabilities = core.getPhysicalDevice().populateSurfaceCapabilities(surface.getSurfaceHandle());

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
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const auto &[graphicsFamily, presentFamily] = core.getPhysicalDevice().getQueueFamilyIndices();
        std::array queueFamilyIndices = {graphicsFamily.value(), presentFamily.value()};

        const VkSurfaceCapabilitiesKHR capabilities = core.getPhysicalDevice().populateSurfaceCapabilities(surface.getSurfaceHandle());

        if (const VkSwapchainCreateInfoKHR createInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = surface.getSurfaceHandle(),
            .minImageCount = maxFramesInFlight,
            .imageFormat = config::SWAPCHAIN_IMAGE_FORMAT,
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
        }; allocator::invoke(vkCreateSwapchainKHR, core.getLogicalDevice().getLogicalDeviceHandle(), &createInfo, nullptr, target) !=
           VK_SUCCESS) {
            throw exception_init("Failed to create swapchain!");
        }
    }

    void SwapchainImpl::setupSwapchainImages() {
        uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        swapchainImages.resize(maxFramesInFlight);
        swapchainImageViews.resize(maxFramesInFlight);

        vkGetSwapchainImagesKHR(core.getLogicalDevice().getLogicalDeviceHandle(), swapchain, &maxFramesInFlight,
                                swapchainImages.data());
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            BufferImageUtils::createImageView(core.getLogicalDevice().getLogicalDeviceHandle(), swapchainImages[i],
                                         VK_IMAGE_VIEW_TYPE_2D, config::SWAPCHAIN_IMAGE_FORMAT, &swapchainImageViews[i]);
        }
    }


    void SwapchainImpl::destroy() {
        destroyImageViews();
        allocator::invoke(vkDestroySwapchainKHR, core.getLogicalDevice().getLogicalDeviceHandle(), swapchain, nullptr);
    }

    void SwapchainImpl::destroyImageViews() const {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            allocator::invoke(vkDestroyImageView, core.getLogicalDevice().getLogicalDeviceHandle(), swapchainImageViews[i], nullptr);
        }
    }
}
