//
// Created by Merutilm on 2025-07-21.
//

#include "ScopedNewCommandBufferExecutor.hpp"

#include "../core/vkh_core.hpp"
#include "../impl/Fence.hpp"

namespace merutilm::vkh {
    ScopedNewCommandBufferExecutor::ScopedNewCommandBufferExecutor(CoreRef core, CommandPoolRef commandPool,
                                                                   FencePtr const fence) : CoreHandler(core),
        commandPool(commandPool), fence(fence) {
        ScopedNewCommandBufferExecutor::init();
    }

    ScopedNewCommandBufferExecutor::~ScopedNewCommandBufferExecutor() {
        ScopedNewCommandBufferExecutor::destroy();
    }

    void ScopedNewCommandBufferExecutor::init() {
        if (const VkCommandBufferAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = commandPool.getCommandPoolHandle(),
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };
            allocator::invoke(vkAllocateCommandBuffers, core.getLogicalDevice().getLogicalDeviceHandle(), &allocInfo,
                                     &commandBuffer) != VK_SUCCESS) {
            throw exception_init("Failed to allocate command buffers!");
        }

        constexpr VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw exception_init("Failed to begin command buffer operation.");
        }
    }

    void ScopedNewCommandBufferExecutor::destroy() {
        vkEndCommandBuffer(commandBuffer);
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr
        };

        core.getLogicalDevice().queueSubmit(1, &submitInfo, fence == nullptr ? nullptr : fence->getFenceHandle());
        if (fence == nullptr) {
            core.getLogicalDevice().waitDeviceIdle();
        } else {
            fence->wait();
        }
        allocator::invoke(vkFreeCommandBuffers, device, commandPool.getCommandPoolHandle(), 1, &commandBuffer);
    }
}
