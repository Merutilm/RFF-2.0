//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/engine/cmd/CommandPool.hpp>

#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    CommandPool::CommandPool(Core & core) : CoreHandler(core) {
        CommandPool::init();
    }

    CommandPool::~CommandPool() {
        CommandPool::cleanup();
    }


    void CommandPool::init() {
        const VkCommandPoolCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = core.getPhysicalDeviceLoader().getQueueFamilyIndices().graphicsAndComputeFamily.
            value(),
        };
        if (vkCreateCommandPool(core.getLogicalDevice().getLogicalDeviceHandle(), &createInfo, nullptr, &commandPool)
            != VK_SUCCESS) {
            throw exception_init("Failed to create command pool!");
        }
    }

    void CommandPool::cleanup() {
        vkDestroyCommandPool(core.getLogicalDevice().getLogicalDeviceHandle(), commandPool, nullptr);
    }
}
