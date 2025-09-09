//
// Created by Merutilm on 2025-07-09.
//

#include "CommandPool.hpp"

#include "../core/vkh_core.hpp"

namespace merutilm::vkh {
    CommandPoolImpl::CommandPoolImpl(CoreRef core) : CoreHandler(core) {
        CommandPoolImpl::init();
    }

    CommandPoolImpl::~CommandPoolImpl() {
        CommandPoolImpl::destroy();
    }


    void CommandPoolImpl::init() {
        const VkCommandPoolCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = core.getPhysicalDevice().getQueueFamilyIndices().graphicsAndComputeFamily.
            value(),
        };
        if (allocator::invoke(vkCreateCommandPool,core.getLogicalDevice().getLogicalDeviceHandle(), &createInfo, nullptr, &commandPool)
            != VK_SUCCESS) {
            throw exception_init("Failed to create command pool!");
        }
    }

    void CommandPoolImpl::destroy() {
        allocator::invoke(vkDestroyCommandPool, core.getLogicalDevice().getLogicalDeviceHandle(), commandPool, nullptr);
    }
}
