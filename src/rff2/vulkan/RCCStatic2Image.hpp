//
// Created by Merutilm on 2025-09-08.
//

#pragma once
#include "SharedImageContextIndices.hpp"
#include "../../vulkan_helper/configurator/RenderContextConfigurator.hpp"
#include "../../vulkan_helper/util/BufferImageContextUtils.hpp"

namespace merutilm::rff2 {
    struct RCCStatic2Image final : public vkh::RenderContextConfiguratorAbstract {
        static constexpr uint32_t CONTEXT_INDEX = 6;
        static constexpr uint32_t SUBPASS_STATIC_IMAGE_INDEX = 0;
        static constexpr uint32_t RESULT_COLOR_ATTACHMENT_INDEX = 0;


        using RenderContextConfiguratorAbstract::RenderContextConfiguratorAbstract;


        void configure(vkh::RenderPassManagerRef rpm) override {
            using namespace SharedImageContextIndices;
            rpm.appendAttachment(RESULT_COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = sharedImageContext.getImageContextMF(MF_VIDEO_RENDER_IMAGE_SECONDARY)[0].
                                     imageFormat,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_GENERAL,
                                 }, sharedImageContext.getImageContextMF(MF_VIDEO_RENDER_IMAGE_SECONDARY));


            rpm.appendSubpass(SUBPASS_STATIC_IMAGE_INDEX);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR,
                                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        }
    };
}
