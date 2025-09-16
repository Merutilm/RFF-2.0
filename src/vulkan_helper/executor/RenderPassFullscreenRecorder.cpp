//
// Created by Merutilm on 2025-07-15.
//

#include "RenderPassFullscreenRecorder.hpp"


#include "../context/RenderContext.hpp"

namespace merutilm::vkh {
    RenderPassFullscreenRecorder::RenderPassFullscreenRecorder(WindowContextRef wc,
                                                               const uint32_t renderContextIndex,
                                                               const uint32_t frameIndex,
                                                               const uint32_t swapchainImageIndex) : WindowContextHandler(wc),
        renderContextIndex(renderContextIndex),
        frameIndex(frameIndex), swapchainImageIndex(swapchainImageIndex) {
        RenderPassFullscreenRecorder::init();
    }

    RenderPassFullscreenRecorder::~RenderPassFullscreenRecorder() {
        RenderPassFullscreenRecorder::destroy();
    }

    void RenderPassFullscreenRecorder::execute(const uint32_t frameIndex,
                                               const std::span<PipelineConfiguratorAbstract * const> shaderPrograms,
                                               std::vector<DescIndexPicker> &&descIndices) const {
        safe_array::check_size_equal(shaderPrograms.size(), descIndices.size(),
                                     "Execution of the Render Pass Fullscreen Recorder");
        const auto cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
        for (int i = 0; i < shaderPrograms.size(); ++i) {
            shaderPrograms[i]->cmdRender(cbh, frameIndex, std::move(descIndices[i]));
            if (i < shaderPrograms.size() - 1) {
                vkCmdNextSubpass(cbh, VK_SUBPASS_CONTENTS_INLINE);
            }
        }
    }


    void RenderPassFullscreenRecorder::cmdMatchViewportAndScissor() const {
        const auto cbh = wc.getCommandBuffer().getCommandBufferHandle(frameIndex);
        const VkExtent2D extent = wc.getRenderContext(renderContextIndex).getFramebuffer()->getExtent();
        const auto [width, height] = extent;
        const VkViewport viewport = {
            .x = 0,
            .y = 0,
            .width = static_cast<float>(width),
            .height = static_cast<float>(height),
            .minDepth = 0,
            .maxDepth = 1
        };
        const VkRect2D scissor = {
            .offset = {0, 0},
            .extent = {width, height}
        };


        vkCmdSetViewport(cbh, 0, 1, &viewport);
        vkCmdSetScissor(cbh, 0, 1, &scissor);
    }


    void RenderPassFullscreenRecorder::init() {
        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        RenderContextRef context = wc.getRenderContext(renderContextIndex);

        const VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = context.getRenderPass()->getRenderPassHandle(),
            .framebuffer = context.getFramebuffer()->getFramebufferHandle(
                swapchainImageIndex == UINT32_MAX ? frameIndex : swapchainImageIndex),
            .renderArea = {
                .offset = {0, 0},
                .extent = context.getFramebuffer()->getExtent()
            },
            .clearValueCount = static_cast<uint32_t>(clearValues.size()),
            .pClearValues = clearValues.data()
        };
        vkCmdBeginRenderPass(wc.getCommandBuffer().getCommandBufferHandle(frameIndex), &renderPassBeginInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderPassFullscreenRecorder::destroy() {
        vkCmdEndRenderPass(wc.getCommandBuffer().getCommandBufferHandle(frameIndex));
    }
}
