//
// Created by Merutilm on 2025-08-28.
//

#include "ScopedCommandBufferExecutor.hpp"

#include "../core/logger.hpp"

namespace merutilm::vkh {
    ScopedCommandBufferExecutor::ScopedCommandBufferExecutor(
        WindowContextRef wc, const uint32_t frameIndex, const VkFence fence, const VkSemaphore imageAvailable,
        const VkSemaphore renderFinished) : WindowContextHandler(wc),
                                            frameIndex(frameIndex), fence(fence),
                                            imageAvailable(imageAvailable), renderFinished(renderFinished) {
        ScopedCommandBufferExecutor::init();
    }

    ScopedCommandBufferExecutor::~ScopedCommandBufferExecutor() {
        ScopedCommandBufferExecutor::destroy();
    }


    void ScopedCommandBufferExecutor::init() {
        const VkCommandBuffer cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
        constexpr VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };

        vkResetCommandBuffer(cbh, 0);
        if (vkBeginCommandBuffer(cbh, &beginInfo) != VK_SUCCESS) {
            throw exception_init("Failed to begin command buffer operation.");
        }
    }

    void ScopedCommandBufferExecutor::destroy() {
        const VkCommandBuffer cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
        vkEndCommandBuffer(cbh);

        std::vector<VkPipelineStageFlags> waitPipelineStage = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        const VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = imageAvailable == VK_NULL_HANDLE ? 0 : 1u,
            .pWaitSemaphores = imageAvailable == VK_NULL_HANDLE ? nullptr : &imageAvailable,
            .pWaitDstStageMask = waitPipelineStage.data(),
            .commandBufferCount = 1u,
            .pCommandBuffers = &cbh,
            .signalSemaphoreCount = renderFinished == VK_NULL_HANDLE ? 0 : 1u,
            .pSignalSemaphores = renderFinished == VK_NULL_HANDLE ? nullptr : &renderFinished,
        };
        wc.core.getLogicalDevice().queueSubmit(1, &submitInfo, fence);
    }
}
