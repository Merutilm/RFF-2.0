//
// Created by Merutilm on 2025-09-07.
//

#pragma once
#include "../../vulkan_helper/configurator/RenderContextConfigurator.hpp"

namespace merutilm::rff2 {
    struct RCCDownsampleForBlurVid final : public vkh::RenderContextConfiguratorAbstract {
        static constexpr uint32_t CONTEXT_INDEX = 1;

        static constexpr uint32_t SUBPASS_DOWNSAMPLE_INDEX = 0;

        static constexpr uint32_t RESULT_COLOR_ATTACHMENT_INDEX = 0;

        using RenderContextConfiguratorAbstract::RenderContextConfiguratorAbstract;

        void configure(vkh::RenderPassManagerRef rpm) override {
            using namespace SharedImageContextIndices;
            rpm.appendAttachment(RESULT_COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = sharedImageContext.getImageContextMF(
                                         MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_PRIMARY)[0].imageFormat,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_GENERAL,
                                 }, sharedImageContext.getImageContextMF(MF_VIDEO_RENDER_DOWNSAMPLED_IMAGE_PRIMARY));


            rpm.appendSubpass(SUBPASS_DOWNSAMPLE_INDEX);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            rpm.appendDependency({
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = SUBPASS_DOWNSAMPLE_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dependencyFlags = 0
            });

            rpm.appendDependency({
                .srcSubpass = SUBPASS_DOWNSAMPLE_INDEX,
                .dstSubpass = VK_SUBPASS_EXTERNAL,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = 0,
                .dependencyFlags = 0
            });
        }
    };
}
