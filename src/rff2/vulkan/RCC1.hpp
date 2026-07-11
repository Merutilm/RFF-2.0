//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include "GPCStripe.hpp"
#include "SharedImageContextIndices.hpp"
#include "vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp"

namespace merutilm::rff2 {
    class RCC1 final : public vkh::RenderPassGraphGenerator {

        vkh::RenderPassAttachment *resultColorAttachment = nullptr;

    public:
        GPCStripe *stripe;

        using RenderPassGraphGenerator::RenderPassGraphGenerator;

    protected:
        void configureAttachments() override {
            using namespace SharedImageContextIndices;
            resultColorAttachment = &appendAttachment(
                    {
                            .flags = 0,
                            .format = wc.getSharedImageContext()
                                              .getImageContextMF(MF_MAIN_RENDER_IMAGE_PRIMARY)[0]
                                              .imageFormat,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_IMAGE_PRIMARY));
        }

        void configurePipelines() override {
            using namespace SharedImageContextIndices;
            registerPipeline<GPCStripe>(&stripe, {},
                                    {resultColorAttachment,
                                     {vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                                     vkh::SubpassDependency::none(),
                                     vkh::RenderPassAttachmentReference::none()},
                                    [] { return vkh::DescIndexPicker{}; });
        }
    };
} // namespace merutilm::rff2
