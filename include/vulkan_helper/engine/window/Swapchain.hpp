//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>
#include "Surface.hpp"


namespace merutilm::vkh {
    class Swapchain final : public CoreHandler {

        Surface &surface;
        VkSwapchainKHR swapchain = nullptr;
        VkSwapchainKHR oldSwapchain = nullptr;
        std::vector<VkImage> swapchainImages = {};
        std::vector<VkImageView> swapchainImageViews = {};
        VkExtent2D swapchainExtent = {};

    public:
        explicit Swapchain(Core &core, Surface &surface);

        ~Swapchain() override;

        Swapchain(const Swapchain &) = delete;

        Swapchain &operator=(const Swapchain &) = delete;

        Swapchain(Swapchain &&) = delete;

        Swapchain &operator=(Swapchain &&) = delete;

        void recreate(VkExtent2D extent);

        [[nodiscard]] VkSwapchainKHR getSwapchainHandle() const { return swapchain; }

        [[nodiscard]] std::span<const VkImage> getSwapchainImages() const { return swapchainImages; }

        [[nodiscard]] std::span<const VkImageView> getSwapchainImageViews() const { return swapchainImageViews; }


        [[nodiscard]] VkExtent2D getSwapchainExtent() const { return swapchainExtent; }

        void updateSwapchainExtent(VkExtent2D extent);


        void createSwapchain(VkSwapchainKHR *target, VkSwapchainKHR old, VkExtent2D extent);

        void setupSwapchainImages();

        void destroyImageViews() const;

    protected:
        void init() override;

        void cleanup() override;
    };


} // namespace merutilm::vkh
