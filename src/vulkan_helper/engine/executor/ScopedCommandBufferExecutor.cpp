//
// Created by Merutilm on 2025-08-28.
//

#include <vulkan_helper/engine/executor/ScopedCommandBufferExecutor.hpp>

namespace merutilm::vkh {
    ScopedCommandBufferExecutor::ScopedCommandBufferExecutor(
        WindowContext & wc, const VkCommandBuffer commandBuffer, const VkFence fence, const VkSemaphore wait,
        const VkSemaphore signal) : WindowContextHandler(wc),
                                            commandBuffer(commandBuffer), fence(fence),
                                            wait(wait), signal(signal) {
        ScopedCommandBufferExecutor::init();
    }

    ScopedCommandBufferExecutor::~ScopedCommandBufferExecutor() {
        ScopedCommandBufferExecutor::cleanup();
    }


    void ScopedCommandBufferExecutor::init() {
        constexpr VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        vkResetCommandBuffer(commandBuffer, 0);
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw exception_init("Failed to begin command buffer operation.");
        }
    }

    void ScopedCommandBufferExecutor::cleanup() {
        vkEndCommandBuffer(commandBuffer);

        std::vector<VkPipelineStageFlags> waitPipelineStage = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        const VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = wait == VK_NULL_HANDLE ? 0 : 1u,
            .pWaitSemaphores = wait == VK_NULL_HANDLE ? nullptr : &wait,
            .pWaitDstStageMask = waitPipelineStage.data(),
            .commandBufferCount = 1u,
            .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = signal == VK_NULL_HANDLE ? 0 : 1u,
            .pSignalSemaphores = signal == VK_NULL_HANDLE ? nullptr : &signal,
        };
        wc.core.getLogicalDevice().queueSubmit(1, &submitInfo, fence);
    }
}
