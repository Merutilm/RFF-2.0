//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/base/config.hpp>
#include <vulkan_helper/base/exception.hpp>
#include <vulkan_helper/core/LogicalDevice.hpp>
#include <vulkan_helper/util/Debugger.hpp>
#include <vulkan_helper/util/PhysicalDeviceUtils.hpp>

namespace merutilm::vkh {
    LogicalDevice::LogicalDevice(Instance &instance, PhysicalDeviceLoader &physicalDevice) :
        instance(instance), physicalDevice(physicalDevice) {
        LogicalDevice::init();
    }

    LogicalDevice::~LogicalDevice() { LogicalDevice::cleanup(); }

    void LogicalDevice::queueSubmit(const uint32_t submitCount, const VkSubmitInfo *pSubmits, const VkFence fence) {
        std::scoped_lock lock(queueMutex);
        VkResult result;
        if ((result = vkQueueSubmit(graphicsQueue, submitCount, pSubmits, fence)) != VK_SUCCESS) {
            throw exception_invalid_state("Failed to submit queue! " + std::to_string(result));
        }
    }

    void LogicalDevice::queuePresent(const VkPresentInfoKHR *presentInfo) {
        std::scoped_lock lock(queueMutex);
        const VkResult result = vkQueuePresentKHR(presentQueue, presentInfo);
        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
            logger::log("warning : the surface is suboptimal or out of date. your window resizing request is so fast!");
        } else if (result != VK_SUCCESS) {
            throw exception_invalid_state("Failed to present queue! " + std::to_string(result));
        }
    }

    void LogicalDevice::waitDeviceIdle() {
        std::scoped_lock lock(queueMutex);
        vkDeviceWaitIdle(logicalDevice);
    }

    void LogicalDevice::init() {
        float queuePriority = 1;
        VkDeviceQueueCreateInfo queueCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = physicalDevice.getQueueFamilyIndices().graphicsAndComputeFamily.value(),
                .queueCount = 1,
                .pQueuePriorities = &queuePriority,

        };
        const VkDeviceCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueCreateInfoCount = 1,
                .pQueueCreateInfos = &queueCreateInfo,
                .enabledLayerCount = config::ENABLE_VALIDATION ? 1 : 0,
                .ppEnabledLayerNames = config::ENABLE_VALIDATION ? &Debugger::VALIDATION_LAYER : nullptr,
                .enabledExtensionCount = static_cast<uint32_t>(PhysicalDeviceUtils::PHYSICAL_DEVICE_EXTENSIONS.size()),
                .ppEnabledExtensionNames = PhysicalDeviceUtils::PHYSICAL_DEVICE_EXTENSIONS.data(),
                .pEnabledFeatures = &physicalDevice.getPhysicalDeviceFeatures()};

        if (vkCreateDevice(physicalDevice.getPhysicalDeviceHandle(), &createInfo, nullptr, &logicalDevice) !=
            VK_SUCCESS) {
            throw exception_init("failed to create logical device!");
        }
        vkGetDeviceQueue(logicalDevice, physicalDevice.getQueueFamilyIndices().graphicsAndComputeFamily.value(), 0,
                         &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, physicalDevice.getQueueFamilyIndices().presentFamily.value(), 0, &presentQueue);
    }


    void LogicalDevice::cleanup() {
        std::scoped_lock lock(queueMutex);
        vkDeviceWaitIdle(logicalDevice);
        vkDestroyDevice(logicalDevice, nullptr);
    }
} // namespace merutilm::vkh
