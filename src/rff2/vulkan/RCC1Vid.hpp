//
// Created by Merutilm on 2025-09-07.
//

#pragma once
#include "SharedImageContextIndices.hpp"
#include "../../vulkan_helper/configurator/RenderContextConfigurator.hpp"

namespace merutilm::rff2 {
    struct RCC1Vid final : public vkh::RenderContextConfiguratorAbstract {
        static constexpr uint32_t CONTEXT_INDEX = 0;
        static constexpr uint32_t SUBPASS_SLOPE_INDEX = 0;
        static constexpr uint32_t SUBPASS_COLOR_INDEX = 1;

        static constexpr uint32_t RESULT_COLOR_ATTACHMENT_INDEX = 0;
        static constexpr uint32_t TEMP_COLOR_ATTACHMENT_INDEX = 1;

        using RenderContextConfiguratorAbstract::RenderContextConfiguratorAbstract;

        void configure(vkh::RenderPassManagerRef rpm) override {
            using namespace SharedImageContextIndices;
            rpm.appendAttachment(RESULT_COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = sharedImageContext.getImageContextMF(MF_VIDEO_RENDER_IMAGE_PRIMARY)[0].
                                     imageFormat,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 }, sharedImageContext.getImageContextMF(MF_VIDEO_RENDER_IMAGE_PRIMARY));

            rpm.appendAttachment(TEMP_COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = sharedImageContext.getImageContextMF(MF_VIDEO_RENDER_IMAGE_SECONDARY)[0].
                                     imageFormat,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 }, sharedImageContext.getImageContextMF(MF_VIDEO_RENDER_IMAGE_SECONDARY));


            rpm.appendSubpass(SUBPASS_SLOPE_INDEX);
            rpm.appendReference(TEMP_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            rpm.appendSubpass(SUBPASS_COLOR_INDEX);
            rpm.appendReference(TEMP_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::INPUT,
                                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            rpm.appendDependency({
                .srcSubpass = SUBPASS_SLOPE_INDEX,
                .dstSubpass = SUBPASS_COLOR_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
                .dependencyFlags = 0
            });
        }
    };
}
