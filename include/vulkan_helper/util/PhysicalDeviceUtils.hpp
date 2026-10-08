//
// Created by Merutilm on 2025-08-24.
//

#pragma once
#include <vulkan_helper/core/QueueFamilyIndices.hpp>
#include <vulkan_helper/hash/StringHasher.hpp>

namespace merutilm::vkh {
    struct PhysicalDeviceUtils {

        explicit PhysicalDeviceUtils() = delete;

        static bool isDeviceSuitable(VkPhysicalDevice physicalDevice, const std::vector<const char *> &extensions,
                                     VkSurfaceKHR surface);

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);

        static bool checkDeviceExtensionSupport(VkPhysicalDevice physicalDevice,
                                                const std::vector<const char *> &extensions);

    };
}
