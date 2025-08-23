//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <span>

#include "../handle/Handler.hpp"

#include "LogicalDevice.hpp"

namespace merutilm::vkh {
    class SwapchainImpl final : public Handler {

        SurfaceRef surface;
        PhysicalDeviceLoaderRef physicalDevice;
        LogicalDeviceRef logicalDevice;

        VkSwapchainKHR swapchain = nullptr;
        VkSwapchainKHR oldSwapchain = nullptr;
        std::vector<VkImage> swapchainImages = {};
        std::vector<VkImageView> swapchainImageViews = {};

    public:
        explicit SwapchainImpl(SurfaceRef surface, PhysicalDeviceLoaderRef physicalDevice,
                           LogicalDeviceRef logicalDevice);

        ~SwapchainImpl() override;

        SwapchainImpl(const SwapchainImpl &) = delete;

        SwapchainImpl &operator=(const SwapchainImpl &) = delete;

        SwapchainImpl(SwapchainImpl &&) = delete;

        SwapchainImpl &operator=(SwapchainImpl &&) = delete;

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

    using Swapchain = std::unique_ptr<SwapchainImpl>;
    using SwapchainPtr = SwapchainImpl *;
    using SwapchainRef = SwapchainImpl &;
}
