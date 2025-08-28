//
// Created by Merutilm on 2025-08-28.
//

#include "ScopedCommandBufferExecutor.hpp"

#include "../util/logger.hpp"

namespace merutilm::vkh {

    ScopedCommandBufferExecutor::ScopedCommandBufferExecutor(EngineRef engine, const uint32_t frameIndex) : engine(engine), frameIndex(frameIndex){
        ScopedCommandBufferExecutor::begin();
    }

    ScopedCommandBufferExecutor::~ScopedCommandBufferExecutor() {
        ScopedCommandBufferExecutor::end();
    }


    void ScopedCommandBufferExecutor::begin() {
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
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

    void ScopedCommandBufferExecutor::end() {
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        VkSemaphore imageAvailableSemaphore = engine.getSyncObject().getImageAvailableSemaphore(frameIndex);
        VkSemaphore renderFinishedSemaphore = engine.getSyncObject().getRenderFinishedSemaphore(frameIndex);
        std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        vkEndCommandBuffer(cbh);
        const VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &imageAvailableSemaphore,
            .pWaitDstStageMask = waitStages.data(),
            .commandBufferCount = 1,
            .pCommandBuffers = &cbh,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &renderFinishedSemaphore
        };

        if (const VkResult result = vkQueueSubmit(engine.getCore().getLogicalDevice().getGraphicsQueue(), 1, &submitInfo,
                                                  engine.getSyncObject().getFence(frameIndex)); result != VK_SUCCESS) {
            logger::log_err_silent("Failed to submit queue!! | CAUSE : {}", static_cast<uint32_t>(result));
                                                  }
    }

}
