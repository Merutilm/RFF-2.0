//
// Created by Merutilm on 2025-07-15.
//

#include "RenderPassExecutor.hpp"

#include <iostream>

#include "../context/RenderContext.hpp"

namespace merutilm::vkh {
    RenderPassExecutor::RenderPassExecutor(const Engine &engine, const RenderContext &renderContext,
                                           const VkExtent2D extent,
                                           const uint32_t frameIndex,
                                           const uint32_t imageIndex) : Executor(engine.getCore()), engine(engine),
                                                                        renderContext(renderContext), extent(extent),
                                                                        frameIndex(frameIndex), imageIndex(imageIndex) {
        RenderPassExecutor::begin();
    }

    RenderPassExecutor::~RenderPassExecutor() {
        RenderPassExecutor::end();
    }


    void RenderPassExecutor::begin() {
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        constexpr VkCommandBufferBeginInfo beginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr
        };
        vkResetCommandBuffer(cbh, 0);
        if (vkBeginCommandBuffer(cbh, &beginInfo) != VK_SUCCESS) {
            throw exception_init("Failed to begin command buffer transmission");
        }

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};

        const VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = renderContext.renderPass->getRenderPassHandle(),
            .framebuffer = renderContext.framebuffer->getFramebufferHandle(imageIndex),
            .renderArea = {
                .offset = {0, 0},
                .extent = extent
            },
            .clearValueCount = static_cast<uint32_t>(clearValues.size()),
            .pClearValues = clearValues.data()
        };

        vkCmdBeginRenderPass(cbh, &renderPassBeginInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderPassExecutor::end() {
        const VkCommandBuffer cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        vkCmdEndRenderPass(cbh);


        VkCommandBuffer commandBufferHandle = cbh;
        VkSemaphore imageAvailableSemaphore = engine.getSyncObject().getImageAvailableSemaphore(frameIndex);
        VkSemaphore renderFinishedSemaphore = engine.getSyncObject().getRenderFinishedSemaphore(frameIndex);
        std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        vkEndCommandBuffer(commandBufferHandle);
        const VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &imageAvailableSemaphore,
            .pWaitDstStageMask = waitStages.data(),
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBufferHandle,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &renderFinishedSemaphore
        };

        if (const VkResult result = vkQueueSubmit(core.getLogicalDevice().getGraphicsQueue(), 1, &submitInfo,
                                                  engine.getSyncObject().getFence(frameIndex)); result != VK_SUCCESS) {
            std::cerr << "Failed to submit queue!! | CAUSE : " << result << std::endl;
        }
    }
}
