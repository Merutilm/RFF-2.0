//
// Created by Merutilm on 2025-07-21.
//

#include "ScopedCommandExecutor.hpp"

#include <iostream>

namespace merutilm::vkh {
    ScopedCommandExecutor::ScopedCommandExecutor(const Core &core, const CommandPool &commandPool) : Executor(core), commandPool(commandPool) {
        ScopedCommandExecutor::begin();
    }

    ScopedCommandExecutor::~ScopedCommandExecutor() {
        ScopedCommandExecutor::end();
    }

    void ScopedCommandExecutor::begin() {
        if (const VkCommandBufferAllocateInfo allocInfo = {
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .pNext = nullptr,
                .commandPool = commandPool.getCommandPoolHandle(),
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
            };
            vkAllocateCommandBuffers(core.getLogicalDevice().getLogicalDeviceHandle(), &allocInfo,
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

    void ScopedCommandExecutor::end() {
        vkEndCommandBuffer(commandBuffer);

        if (const VkSubmitInfo submitInfo = {
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
            vkQueueSubmit(core.getLogicalDevice().getGraphicsQueue(), 1, &submitInfo, nullptr) != VK_SUCCESS) {
            std::cerr << "Failed to submit command buffer operation.\n" << std::flush;
        }
        vkDeviceWaitIdle(core.getLogicalDevice().getLogicalDeviceHandle());
        vkFreeCommandBuffers(core.getLogicalDevice().getLogicalDeviceHandle(), commandPool.getCommandPoolHandle(), 1, &commandBuffer);
    }
}
