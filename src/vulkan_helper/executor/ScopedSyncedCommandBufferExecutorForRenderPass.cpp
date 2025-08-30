//
// Created by Merutilm on 2025-08-28.
//

#include "ScopedSyncedCommandBufferExecutorForRenderPass.hpp"

#include "../util/logger.hpp"

namespace merutilm::vkh {
    ScopedSyncedCommandBufferExecutorForRenderPass::ScopedSyncedCommandBufferExecutorForRenderPass(
        EngineRef engine, const uint32_t frameIndex,
        const RenderPassProcessTypeFlags renderPassProcessType) : engine(engine),
                                                             frameIndex(frameIndex),
                                                             renderPassProcessType(renderPassProcessType) {
        ScopedSyncedCommandBufferExecutorForRenderPass::begin();
    }

    ScopedSyncedCommandBufferExecutorForRenderPass::~ScopedSyncedCommandBufferExecutorForRenderPass() {
        ScopedSyncedCommandBufferExecutorForRenderPass::end();
    }


    void ScopedSyncedCommandBufferExecutorForRenderPass::begin() {
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        const VkFence rpFence = engine.getSyncObjectBetweenRenderPass().getFence(frameIndex);
        vkResetFences(engine.getCore().getLogicalDevice().getLogicalDeviceHandle(), 1, &rpFence);
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

    void ScopedSyncedCommandBufferExecutorForRenderPass::end() {
        using enum RenderPassProcessTypeFlagBits;
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        const VkFence rpFence = engine.getSyncObjectBetweenRenderPass().getFence(frameIndex);
        const VkFence frFence = engine.getSyncObjectBetweenFrame().getFence(frameIndex);
        const VkSemaphore imageAvailableSemaphore = engine.getSyncObjectBetweenFrame().
                getImageAvailableSemaphore(frameIndex);
        const VkSemaphore renderFinishedSemaphore = engine.getSyncObjectBetweenFrame().
                getRenderFinishedSemaphore(frameIndex);
        vkEndCommandBuffer(cbh);

        std::vector<VkPipelineStageFlags> waitPipelineStage = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        const VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = renderPassProcessType & FIRST ? static_cast<uint32_t>(1) : 0,
            .pWaitSemaphores = renderPassProcessType & FIRST ? &imageAvailableSemaphore : nullptr,
            .pWaitDstStageMask = renderPassProcessType & FIRST ? waitPipelineStage.data() : nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &cbh,
            .signalSemaphoreCount = renderPassProcessType & LAST ? static_cast<uint32_t>(1) : 0,
            .pSignalSemaphores = renderPassProcessType & LAST ? &renderFinishedSemaphore : nullptr
        };

        if (const VkResult result = vkQueueSubmit(engine.getCore().getLogicalDevice().getGraphicsQueue(), 1,
                                                  &submitInfo, renderPassProcessType & LAST ? frFence : rpFence); result != VK_SUCCESS) {
            logger::log_err_silent("Failed to submit queue!! | CAUSE : {}", static_cast<uint32_t>(result));
        }
        if (renderPassProcessType != LAST) {
            vkWaitForFences(engine.getCore().getLogicalDevice().getLogicalDeviceHandle(), 1, &rpFence, VK_TRUE,
                            UINT64_MAX);
        }
    }
}
