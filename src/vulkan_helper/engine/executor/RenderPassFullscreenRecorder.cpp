//
// Created by Merutilm on 2025-07-15.
//

#include <vulkan_helper/engine/executor/RenderPassFullscreenRecorder.hpp>


#include <vulkan_helper/engine/context/RenderContext.hpp>

#include "vulkan_helper/engine/configurator/GraphicsPipelineConfigurator.hpp"
#include "vulkan_helper/engine/pipeline/GraphicsPipelineNode.hpp"

namespace merutilm::vkh {
    RenderPassFullscreenRecorder::RenderPassFullscreenRecorder(WindowContext &wc, RenderContext &rc,
                                                               const uint32_t frameIndex,
                                                               const uint32_t swapchainImageIndex) :
        WindowContextHandler(wc), rc(rc), frameIndex(frameIndex), swapchainImageIndex(swapchainImageIndex) {
        RenderPassFullscreenRecorder::init();
    }

    RenderPassFullscreenRecorder::~RenderPassFullscreenRecorder() { RenderPassFullscreenRecorder::cleanup(); }

    void RenderPassFullscreenRecorder::execute(const uint32_t frameIndex) const {

        const auto cbh = wc.getCommandBufferGroup().getCommandBuffer(frameIndex).getCommandBufferHandle();
        uint32_t prevSubpass = 0;
        for (const auto &gpn: rc.getGenerator()->pipelineNodes) {
            const uint32_t currentSubpass = gpn->getSubpass();
            if (currentSubpass - prevSubpass >= 2)
                throw exception_invalid_state("empty subpass");
            if (currentSubpass > prevSubpass) {
                vkCmdNextSubpass(cbh, VK_SUBPASS_CONTENTS_INLINE);
            }
            gpn->getPipelineConfigurator().cmdRender(cbh, frameIndex, gpn->genPicker());
            prevSubpass = currentSubpass;
        }
    }


    void RenderPassFullscreenRecorder::cmdMatchViewportAndScissor() const {
        const auto cbh = wc.getCommandBufferGroup().getCommandBuffer(frameIndex).getCommandBufferHandle();
        const VkExtent2D extent = rc.getFramebuffer()->getExtent();
        const auto [width, height] = extent;
        const VkViewport viewport = {.x = 0,
                                     .y = 0,
                                     .width = static_cast<float>(width),
                                     .height = static_cast<float>(height),
                                     .minDepth = 0,
                                     .maxDepth = 1};
        const VkRect2D scissor = {.offset = {0, 0}, .extent = {width, height}};


        vkCmdSetViewport(cbh, 0, 1, &viewport);
        vkCmdSetScissor(cbh, 0, 1, &scissor);
    }


    void RenderPassFullscreenRecorder::init() {
        std::vector<VkClearValue> clearValues = {};

        for (const auto &attachment: rc.getGenerator()->rpm.attachments) {
            clearValues.push_back(attachment->clearValue);
        }

        rc.getRenderPass()->begin(wc.getCommandBufferGroup().getCommandBuffer(frameIndex).getCommandBufferHandle(),
                                  rc.getFramebuffer()->getFramebufferHandle(
                                          swapchainImageIndex == UINT32_MAX ? frameIndex : swapchainImageIndex),
                                  {0, 0}, rc.getFramebuffer()->getExtent(), clearValues);
        cmdMatchViewportAndScissor();
    }

    void RenderPassFullscreenRecorder::cleanup() {
        RenderPass::end(wc.getCommandBufferGroup().getCommandBuffer(frameIndex).getCommandBufferHandle());
    }
} // namespace merutilm::vkh
