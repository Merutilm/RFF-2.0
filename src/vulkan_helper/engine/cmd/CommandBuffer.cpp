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
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        commandBuffers.resize(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            VkCommandBufferAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = commandPool.getCommandPoolHandle(),
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };
            if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffers[i]) != VK_SUCCESS) {
                throw exception_init("Failed to allocate command buffers!");
            }
        }
    }

    void CommandBuffer::cleanup() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            vkFreeCommandBuffers(device, commandPool.getCommandPoolHandle(), 1, &commandBuffers[i]);
        }
    }



}
