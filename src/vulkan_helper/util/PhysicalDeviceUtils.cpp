//
// Created by Merutilm on 2026-02-03.
//

#include <vulkan_helper/util/PhysicalDeviceUtils.hpp>

namespace merutilm::vkh {
    bool PhysicalDeviceUtils::isDeviceSuitable(const VkPhysicalDevice physicalDevice, const std::vector<const char *> &extensions, const VkSurfaceKHR surface) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);

        const auto indices = findQueueFamilies(physicalDevice, surface);
        return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               features.geometryShader && features.shaderFloat64 &&
               indices.isComplete() &&
               checkDeviceExtensionSupport(physicalDevice, extensions);
    }

    QueueFamilyIndices PhysicalDeviceUtils::findQueueFamilies(const VkPhysicalDevice physicalDevice,
        const VkSurfaceKHR surface) {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (const auto &queueFamily = queueFamilies[i]; (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                indices.graphicsAndComputeFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }
        }
        return indices;
    }

    bool PhysicalDeviceUtils::checkDeviceExtensionSupport(const VkPhysicalDevice physicalDevice, const std::vector<const char *> &extensions) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
        auto required = std::unordered_set<std::string, StringHasher, std::equal_to<>>(
                extensions.begin(), extensions.end());
        for (const auto &[extensionName, specVersion]: availableExtensions) {
            required.erase(extensionName);
        }
        return required.empty();
    }
} // namespace merutilm::vkh
