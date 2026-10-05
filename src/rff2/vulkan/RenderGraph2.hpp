//
// Created by Merutilm on 2025-09-08.
//

#pragma once
#include "../util/RendererUtils.hpp"
#include "GPCColor.hpp"
#include "GPCFractalSurface.hpp"
#include "SharedImageContextIndices.hpp"
#include "vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp"

namespace merutilm::rff2 {
    class RenderGraph2 final : public vkh::RenderPassGraphGenerator {

        vkh::RenderPassAttachment *resultAttachment = nullptr;
        vkh::RenderPassAttachment *tempAttachment = nullptr;

    public:
        GPCFractalSurface *fractalSurface = nullptr;
        GPCColor *color = nullptr;

        using RenderPassGraphGenerator::RenderPassGraphGenerator;

    protected:
        void configureAttachments() override {
            using namespace SharedImageContextIndices;
            tempAttachment = &appendAttachment(
                    VkAttachmentDescription{
                            .flags = 0,
                            .format = wc.getSharedImageContext()
                                              .getImageContextMF(MF_MAIN_RENDER_IMAGE_SECONDARY)[0]
                                              .imageFormat,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_IMAGE_SECONDARY));
            resultAttachment = &appendAttachment(
                    VkAttachmentDescription{
                            .flags = 0,
                            .format = wc.getSharedImageContext()
                                              .getImageContextMF(MF_MAIN_RENDER_IMAGE_PRIMARY)[0]
                                              .imageFormat,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_IMAGE_PRIMARY));
        }


        void configurePipelines() override {

            vkh::GraphicsPipelineNode *surfaceNode =
                    registerPipeline<GPCFractalSurface>(&fractalSurface, {},
                                               {.targetAttachment = tempAttachment, .srcReferenceInfo = RendererUtils::COLOR_REF_INFO,
                                                .dependency = RendererUtils::INPUT_READ_DEPENDENCY, .dstReferenceInfo = RendererUtils::INPUT_REF_INFO},
                                               RendererUtils::DEFAULT_DESC_PICKER);

            registerPipeline<GPCColor>(&color, {surfaceNode},
                                       {.targetAttachment = resultAttachment, .srcReferenceInfo = RendererUtils::COLOR_REF_INFO, .dependency = std::nullopt, .dstReferenceInfo = std::nullopt},
                                       RendererUtils::DEFAULT_DESC_PICKER);
        }
    };
} // namespace merutilm::rff2
