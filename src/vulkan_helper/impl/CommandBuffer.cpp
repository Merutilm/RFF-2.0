//
// Created by Merutilm on 2025-07-09.
//

#include "CommandBuffer.hpp"

#include "../core/vkh_core.hpp"

namespace merutilm::vkh {
    CommandBufferImpl::CommandBufferImpl(CoreRef core, CommandPoolRef commandPool) : CoreHandler(core), commandPool(commandPool) {
        CommandBufferImpl::init();
    }

    CommandBufferImpl::~CommandBufferImpl() {
        CommandBufferImpl::destroy();
    }


    void CommandBufferImpl::init() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        commandBuffers.resize(maxFramesInFlight);
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            VkCommandBufferAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = commandPool.getCommandPoolHandle(),
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };
            if (allocator::invoke(vkAllocateCommandBuffers,device, &allocInfo, &commandBuffers[i]) != VK_SUCCESS) {
                throw exception_init("Failed to allocate command buffers!");
            }
        }
    }

    void CommandBufferImpl::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            allocator::invoke(vkFreeCommandBuffers, device, commandPool.getCommandPoolHandle(), 1, &commandBuffers[i]);
        }
    }



}
