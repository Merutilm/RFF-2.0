//
// Created by Merutilm on 2025-09-07.
//

#pragma once
#include "../../vulkan_helper/configurator/RenderContextConfigurator.hpp"

namespace merutilm::rff2 {
    struct RCC3Vid final : public vkh::RenderContextConfiguratorAbstract {
        static constexpr uint32_t CONTEXT_INDEX = 3;

        static constexpr uint32_t SUBPASS_BLOOM_INDEX = 0;

        static constexpr uint32_t RESULT_COLOR_ATTACHMENT_INDEX = 0;

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

            rpm.appendSubpass(SUBPASS_BLOOM_INDEX);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);


        }
    };
}
