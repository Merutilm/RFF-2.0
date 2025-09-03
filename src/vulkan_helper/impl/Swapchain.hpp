//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class SwapchainImpl final : public CoreHandler {

        SurfaceRef surface;
        VkSwapchainKHR swapchain = nullptr;
        VkSwapchainKHR oldSwapchain = nullptr;
        std::vector<VkImage> swapchainImages = {};
        std::vector<VkImageView> swapchainImageViews = {};

    public:
        explicit SwapchainImpl(CoreRef core, SurfaceRef surface);

        ~SwapchainImpl() override;

        SwapchainImpl(const SwapchainImpl &) = delete;

        SwapchainImpl &operator=(const SwapchainImpl &) = delete;

        SwapchainImpl(SwapchainImpl &&) = delete;

        SwapchainImpl &operator=(SwapchainImpl &&) = delete;

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
