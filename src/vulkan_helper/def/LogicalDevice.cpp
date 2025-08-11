//
// Created by Merutilm on 2025-07-09.
//

#include "LogicalDevice.hpp"

#include <queue>

#include "../exception/exception.hpp"

namespace merutilm::vkh {

    LogicalDevice::LogicalDevice(const Instance& instance, const PhysicalDevice &physicalDevice) :  instance(instance), physicalDevice(physicalDevice) {
        LogicalDevice::init();
    }

    LogicalDevice::~LogicalDevice() {
        LogicalDevice::destroy();
    }

    void LogicalDevice::init() {
        float queuePriority = 1;
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = physicalDevice.getQueueFamilyIndices().graphicsFamily.value(),
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,

        };
        const ValidationLayer &validationLayer = instance.getValidationLayer();

        if (const VkDeviceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueCreateInfo,
            .enabledLayerCount = validationLayer.isEnabled() ? static_cast<uint32_t>(1) : 0,
            .ppEnabledLayerNames = validationLayer.isEnabled() ? &ValidationLayer::VALIDATION_LAYER : nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(PhysicalDevice::PHYSICAL_DEVICE_EXTENSIONS.size()),
            .ppEnabledExtensionNames = PhysicalDevice::PHYSICAL_DEVICE_EXTENSIONS.data(),
            .pEnabledFeatures = &physicalDevice.getPhysicalDeviceFeatures()
        }; vkCreateDevice(physicalDevice.getPhysicalDeviceHandle(), &createInfo, nullptr, &logicalDevice) !=
           VK_SUCCESS) {
            throw exception_init("failed to create logical device!");
        }
        vkGetDeviceQueue(logicalDevice, physicalDevice.getQueueFamilyIndices().graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, physicalDevice.getQueueFamilyIndices().presentFamily.value(), 0, &presentQueue);
    }


    void LogicalDevice::destroy() {
        vkDestroyDevice(logicalDevice, nullptr);
    }
}
