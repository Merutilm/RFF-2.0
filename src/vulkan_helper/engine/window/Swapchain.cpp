//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/engine/window/Swapchain.hpp>

#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/util/BufferImageUtils.hpp>

namespace merutilm::vkh {
    Swapchain::Swapchain(Core & core, Surface & surface) : CoreHandler(core), surface(surface) {
        Swapchain::init();
    }

    Swapchain::~Swapchain() {
        Swapchain::cleanup();
    }


    void Swapchain::recreate(const VkExtent2D extent) {
        destroyImageViews();
        oldSwapchain = swapchain;
        createSwapchain(&swapchain, oldSwapchain, extent);
        vkDestroySwapchainKHR(core.getLogicalDevice().getLogicalDeviceHandle(), oldSwapchain, nullptr);
        setupSwapchainImages();
    }


    void Swapchain::init() {
        createSwapchain(&swapchain, nullptr, surface.getTargetWindow()->calcCurrentExtent());
        setupSwapchainImages();
    }


    void Swapchain::updateSwapchainExtent(const VkExtent2D extent) {
        const auto capabilities =
                core.getPhysicalDeviceLoader().populateSurfaceCapabilities(surface.getSurfaceHandle());

        if (capabilities.currentExtent.width == UINT32_MAX) {
            swapchainExtent = extent;
        }else {
            swapchainExtent = capabilities.currentExtent;
        }
    }


    void Swapchain::createSwapchain(VkSwapchainKHR *target, const VkSwapchainKHR old, const VkExtent2D extent) {
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        const auto &[graphicsFamily, presentFamily] = core.getPhysicalDeviceLoader().getQueueFamilyIndices();
        std::array queueFamilyIndices = {graphicsFamily.value(), presentFamily.value()};

        const VkSurfaceCapabilitiesKHR capabilities = core.getPhysicalDeviceLoader().populateSurfaceCapabilities(surface.getSurfaceHandle());
        updateSwapchainExtent(extent);

        if (const VkSwapchainCreateInfoKHR createInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = surface.getSurfaceHandle(),
            .minImageCount = maxFramesInFlight,
            .imageFormat = core.getPhysicalDeviceLoader().getPrimarySurfaceFormat(),
            .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
            .imageExtent = swapchainExtent,
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
        }; vkCreateSwapchainKHR(core.getLogicalDevice().getLogicalDeviceHandle(), &createInfo, nullptr, target) !=
           VK_SUCCESS) {
            throw exception_init("Failed to create swapchain!");
        }
    }

    void Swapchain::setupSwapchainImages() {
        uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        swapchainImages.resize(maxFramesInFlight);
        swapchainImageViews.resize(maxFramesInFlight);

        vkGetSwapchainImagesKHR(core.getLogicalDevice().getLogicalDeviceHandle(), swapchain, &maxFramesInFlight,
                                swapchainImages.data());


        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            BufferImageUtils::createImageView(core.getLogicalDevice().getLogicalDeviceHandle(), swapchainImages[i],
                                         VK_IMAGE_VIEW_TYPE_2D, core.getPhysicalDeviceLoader().getPrimarySurfaceFormat(), &swapchainImageViews[i]);
        }
    }


    void Swapchain::cleanup() {
        destroyImageViews();
        vkDestroySwapchainKHR(core.getLogicalDevice().getLogicalDeviceHandle(), swapchain, nullptr);
    }

    void Swapchain::destroyImageViews() const {
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            vkDestroyImageView(core.getLogicalDevice().getLogicalDeviceHandle(), swapchainImageViews[i], nullptr);
        }
    }
}
