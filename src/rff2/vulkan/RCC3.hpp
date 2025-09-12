//
// Created by Merutilm on 2025-08-30.
//

#pragma once
#include "SharedImageContextIndices.hpp"
#include "../../vulkan_helper/configurator/RenderContextConfigurator.hpp"

namespace merutilm::rff2 {
    struct RCC3 final : public vkh::RenderContextConfiguratorAbstract {
        static constexpr uint32_t CONTEXT_INDEX = 4;
        static constexpr uint32_t SUBPASS_FOG_INDEX = 0;
        static constexpr uint32_t SUBPASS_BLOOM_THRESHOLD_INDEX = 1;


        static constexpr uint32_t RESULT_COLOR_ATTACHMENT_INDEX = 0;
        static constexpr uint32_t BLOOM_THRESHOLD_COLOR_ATTACHMENT_INDEX = 1;


        using RenderContextConfiguratorAbstract::RenderContextConfiguratorAbstract;

        void configure(vkh::RenderPassManagerRef rpm) override {
            using namespace SharedImageContextIndices;
            rpm.appendAttachment(RESULT_COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = sharedImageContext.getImageContextMF(MF_MAIN_RENDER_IMAGE_SECONDARY)[0].imageFormat,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 }, sharedImageContext.getImageContextMF(MF_MAIN_RENDER_IMAGE_SECONDARY));
            rpm.appendAttachment(BLOOM_THRESHOLD_COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = sharedImageContext.getImageContextMF(MF_MAIN_RENDER_IMAGE_PRIMARY)[0].imageFormat,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 }, sharedImageContext.getImageContextMF(MF_MAIN_RENDER_IMAGE_PRIMARY));

            rpm.appendSubpass(SUBPASS_FOG_INDEX);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            rpm.appendSubpass(SUBPASS_BLOOM_THRESHOLD_INDEX);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::INPUT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            rpm.appendReference(BLOOM_THRESHOLD_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);


            rpm.appendDependency({
                .srcSubpass = SUBPASS_FOG_INDEX,
                .dstSubpass = SUBPASS_BLOOM_THRESHOLD_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
                .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT
            });
        }
    };
}
