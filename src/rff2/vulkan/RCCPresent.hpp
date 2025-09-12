//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "../../vulkan_helper/configurator/RenderContextConfigurator.hpp"

namespace merutilm::rff2 {
    struct RCCPresent final : public vkh::RenderContextConfiguratorAbstract {
        static constexpr uint32_t CONTEXT_INDEX = 7;

        static constexpr uint32_t SUBPASS_PRESENT_INDEX = 0;

        static constexpr uint32_t PRESENT_ATTACHMENT_INDEX = 0;

        using RenderContextConfiguratorAbstract::RenderContextConfiguratorAbstract;

        void configure(vkh::RenderPassManagerRef rpm) override {

            rpm.appendAttachment(PRESENT_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = vkh::config::SWAPCHAIN_IMAGE_FORMAT,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                                 }, swapchainImageContextGetter());

            rpm.appendSubpass(SUBPASS_PRESENT_INDEX);
            rpm.appendReference(PRESENT_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            rpm.appendDependency({
                .srcSubpass = SUBPASS_PRESENT_INDEX,
                .dstSubpass = VK_SUBPASS_EXTERNAL,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = 0,
                .dependencyFlags = 0
            });
        }
    };
}
