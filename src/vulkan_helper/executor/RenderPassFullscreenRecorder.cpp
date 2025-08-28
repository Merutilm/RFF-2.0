//
// Created by Merutilm on 2025-07-15.
//

#include "RenderPassFullscreenRecorder.hpp"


#include "../context/RenderContext.hpp"
#include "../util/logger.hpp"

namespace merutilm::vkh {
    RenderPassFullscreenRecorder::RenderPassFullscreenRecorder(EngineRef engine,
                                                               const uint32_t renderContextIndex,
                                                               const uint32_t frameIndex,
                                                               const uint32_t imageIndex) : engine(engine),
        renderContextIndex(renderContextIndex),
        frameIndex(frameIndex), imageIndex(imageIndex) {
        RenderPassFullscreenRecorder::begin();
    }

    RenderPassFullscreenRecorder::~RenderPassFullscreenRecorder() {
        RenderPassFullscreenRecorder::end();
    }

    void RenderPassFullscreenRecorder::execute(
        const std::span<PipelineConfigurator * const> shaderPrograms, const uint32_t frameIndex) const {
        const auto cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        for (int i = 0; i < shaderPrograms.size(); ++i) {
            shaderPrograms[i]->cmdRender(cbh, frameIndex);
            if (i < shaderPrograms.size() - 1) {
                vkCmdNextSubpass(cbh, VK_SUBPASS_CONTENTS_INLINE);
            }
        }
    }


    void RenderPassFullscreenRecorder::cmdMatchViewportAndScissor() const {
        const auto cbh = engine.getCommandBuffer().getCommandBufferHandle(frameIndex);
        const VkExtent2D extent = engine.getRenderContext(renderContextIndex).getFramebuffer()->getExtent();
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


    void RenderPassFullscreenRecorder::begin() {
        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        RenderContextRef context = engine.getRenderContext(renderContextIndex);

        const VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = context.getRenderPass()->getRenderPassHandle(),
            .framebuffer = context.getFramebuffer()->getFramebufferHandle(imageIndex),
            .renderArea = {
                .offset = {0, 0},
                .extent = context.getFramebuffer()->getExtent()
            },
            .clearValueCount = static_cast<uint32_t>(clearValues.size()),
            .pClearValues = clearValues.data()
        };

        vkCmdBeginRenderPass(engine.getCommandBuffer().getCommandBufferHandle(frameIndex), &renderPassBeginInfo,
                             VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderPassFullscreenRecorder::end() {
        vkCmdEndRenderPass(engine.getCommandBuffer().getCommandBufferHandle(frameIndex));
    }
}
