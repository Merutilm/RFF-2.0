//
// Created by Merutilm on 2025-07-09.
//

#include "LogicalDevice.hpp"

#include <queue>

#include "../core/exception.hpp"
#include "../core/config.hpp"
#include "../util/Debugger.hpp"
#include "../util/PhysicalDeviceUtils.hpp"

namespace merutilm::vkh {
    LogicalDeviceImpl::LogicalDeviceImpl(InstanceRef instance,
                                         PhysicalDeviceLoaderRef physicalDevice) : instance(instance),
        physicalDevice(physicalDevice) {
        LogicalDeviceImpl::init();
    }

    LogicalDeviceImpl::~LogicalDeviceImpl() {
        LogicalDeviceImpl::destroy();
    }

    void LogicalDeviceImpl::init() {
        float queuePriority = 1;
        VkDeviceQueueCreateInfo queueCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = physicalDevice.getQueueFamilyIndices().graphicsAndComputeFamily.value(),
            .queueCount = 1,
            .pQueuePriorities = &queuePriority,

        };

        if (const VkDeviceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &queueCreateInfo,
            .enabledLayerCount = config::ENABLE_VALIDATION ? 1 : 0,
            .ppEnabledLayerNames = config::ENABLE_VALIDATION ? &Debugger::VALIDATION_LAYER : nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(PhysicalDeviceUtils::PHYSICAL_DEVICE_EXTENSIONS.size()),
            .ppEnabledExtensionNames = PhysicalDeviceUtils::PHYSICAL_DEVICE_EXTENSIONS.data(),
            .pEnabledFeatures = &physicalDevice.getPhysicalDeviceFeatures()
        }; allocator::invoke(vkCreateDevice, physicalDevice.getPhysicalDeviceHandle(), &createInfo, nullptr, &logicalDevice) !=
           VK_SUCCESS) {
            throw exception_init("failed to create logical device!");
        }
        vkGetDeviceQueue(logicalDevice, physicalDevice.getQueueFamilyIndices().graphicsAndComputeFamily.value(), 0,
                         &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, physicalDevice.getQueueFamilyIndices().presentFamily.value(), 0, &presentQueue);
    }


    void LogicalDeviceImpl::destroy() {
        allocator::invoke(vkDestroyDevice, logicalDevice, nullptr);
    }
}
