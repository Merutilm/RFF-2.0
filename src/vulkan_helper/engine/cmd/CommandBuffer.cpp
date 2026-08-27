//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/engine/cmd/CommandBuffer.hpp>

#include <vulkan_helper/base/vkh_base.hpp>

namespace merutilm::vkh {
    CommandBuffer::CommandBuffer(Core & core, CommandPool & commandPool) : CoreHandler(core), commandPool(commandPool) {
        CommandBuffer::init();
    }

    CommandBuffer::~CommandBuffer() {
        CommandBuffer::cleanup();
    }

    void CommandBuffer::init() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkCommandBufferAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool.getCommandPoolHandle(),
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
            throw exception_init("Failed to allocate command buffers!");
        }
    }

    void CommandBuffer::cleanup() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        vkFreeCommandBuffers(device, commandPool.getCommandPoolHandle(), 1, &commandBuffer);

    }



}
