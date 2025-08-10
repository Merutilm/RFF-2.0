//
// Created by Merutilm on 2025-07-09.
//

#include "PhysicalDevice.hpp"

#include <unordered_set>

#include "../exception/exception.hpp"
#include "../struct/StringHasher.hpp"

namespace merutilm::mvk {
    PhysicalDevice::PhysicalDevice(const Instance &instance, const Surface &surface) : instance(instance), surface(surface) {
        PhysicalDevice::init();
    }

    PhysicalDevice::~PhysicalDevice() {
        PhysicalDevice::destroy();
    }

    VkSurfaceCapabilitiesKHR PhysicalDevice::populateSurfaceCapabilities() const {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface.getSurfaceHandle(), &surfaceCapabilities);
        return surfaceCapabilities;
    }


    void PhysicalDevice::init() {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance.getInstanceHandle(), &physicalDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance.getInstanceHandle(), &physicalDeviceCount, physicalDevices.data());

        for (const auto pd: physicalDevices) {
            if (isDeviceSuitable(pd, surface.getSurfaceHandle())) {
                physicalDevice = pd;
                vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
                vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
                vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
                queueFamilyIndices = findQueueFamilies(pd, surface.getSurfaceHandle());
                const VkSurfaceCapabilitiesKHR capabilities = populateSurfaceCapabilities();
                maxFramesInFlight = capabilities.minImageCount + 1;
                if (capabilities.maxImageCount > 0 && maxFramesInFlight > capabilities.maxImageCount) {
                    maxFramesInFlight = capabilities.maxImageCount;
                }
                break;
            }
        }
        if (physicalDevice == nullptr) {
            throw exception_init("No suitable physical device found");
        }
    }


    bool PhysicalDevice::isDeviceSuitable(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physicalDevice, &features);

        const auto indices = findQueueFamilies(physicalDevice, surface);
        return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
               features.geometryShader &&
               indices.isComplete() &&
               checkDeviceExtensionSupport(physicalDevice);
    }

    QueueFamilyIndices PhysicalDevice::findQueueFamilies(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface) {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (const auto &queueFamily = queueFamilies[i]; queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
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


    bool PhysicalDevice::checkDeviceExtensionSupport(const VkPhysicalDevice physicalDevice) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());
        auto required = std::unordered_set<std::string, StringHasher, std::equal_to<> >(
            PHYSICAL_DEVICE_EXTENSIONS.begin(),
            PHYSICAL_DEVICE_EXTENSIONS.end());
        for (const auto &[extensionName, specVersion]: availableExtensions) {
            required.erase(extensionName);
        }
        return required.empty();
    }

    void PhysicalDevice::destroy() {
        //nothing to do
    }
}
