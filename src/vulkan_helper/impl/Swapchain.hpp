//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <span>

#include "../handle/Handler.hpp"

#include "LogicalDevice.hpp"

namespace merutilm::vkh {
    class Swapchain final : public Handler {

        const Surface &surface;
        const PhysicalDevice &physicalDevice;
        const LogicalDevice &logicalDevice;

        VkSwapchainKHR swapchain = nullptr;
        VkSwapchainKHR oldSwapchain = nullptr;
        std::vector<VkImage> swapchainImages = {};
        std::vector<VkImageView> swapchainImageViews = {};

    public:
        explicit Swapchain(const Surface &surface, const PhysicalDevice &physicalDevice,
                           const LogicalDevice &logicalDevice);

        ~Swapchain() override;

        Swapchain(const Swapchain &) = delete;

        Swapchain &operator=(const Swapchain &) = delete;

        Swapchain(Swapchain &&) = delete;

        Swapchain &operator=(Swapchain &&) = delete;

        void matchViewportAndScissor(VkCommandBuffer cbh) const;

        void recreate();

        [[nodiscard]] VkExtent2D populateSwapchainExtent() const;

        [[nodiscard]] VkSwapchainKHR getSwapchainHandle() const { return swapchain; }

        [[nodiscard]] std::span<const VkImage> getSwapchainImages() const { return swapchainImages; }

        [[nodiscard]] std::span<const VkImageView> getSwapchainImageViews() const { return swapchainImageViews; }

    private:
        void init() override;

        void createSwapchain(VkSwapchainKHR *target, VkSwapchainKHR old) const;

        void setupSwapchainImages();

        void destroy() override;

        void destroyImageViews() const;
    };
}
