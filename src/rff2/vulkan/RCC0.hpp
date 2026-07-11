//
// Created by Merutilm on 2025-09-08.
//

#pragma once
#include "GPCIterationPalette.hpp"
#include "SharedImageContextIndices.hpp"
#include "vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp"

namespace merutilm::rff2 {
    class RCC0 final : public vkh::RenderPassGraphGenerator {

        vkh::RenderPassAttachment *resultAttachment;

    public:
        GPCIterationPalette *iterationPalette;

        using RenderPassGraphGenerator::RenderPassGraphGenerator;

    protected:
        void configureAttachments() override {
            using namespace SharedImageContextIndices;
            resultAttachment = &appendAttachment(
                    {
                            .flags = 0,
                            .format = wc.getSharedImageContext()
                                              .getImageContextMF(MF_MAIN_RENDER_IMAGE_SECONDARY)[0]
                                              .imageFormat,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_IMAGE_SECONDARY));
        }


        void configurePipelines() override {

            registerPipeline<GPCIterationPalette>(
                    &iterationPalette, {},
                    {resultAttachment,
                     {vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                     vkh::SubpassDependency::none(),
                     vkh::RenderPassAttachmentReference::none()},
                    [] { return vkh::DescIndexPicker{}; });
        }
    };
} // namespace merutilm::rff2
