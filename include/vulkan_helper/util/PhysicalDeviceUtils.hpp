//
// Created by Merutilm on 2025-08-24.
//

#pragma once
#include <vulkan_helper/core/QueueFamilyIndices.hpp>
#include <vulkan_helper/hash/StringHasher.hpp>

namespace merutilm::vkh {
    struct PhysicalDeviceUtils {

        inline static const std::vector<const char *> PHYSICAL_DEVICE_EXTENSIONS = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        explicit PhysicalDeviceUtils() = delete;


        static bool isDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

        static bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice);

    };
}
