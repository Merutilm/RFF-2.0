//
// Created by Merutilm on 2025-09-08.
//

#pragma once
#include "../util/RendererUtils.hpp"
#include "GPC3DFractal.hpp"
#include "GPCColor.hpp"
#include "SharedImageContextIndices.hpp"
#include "vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp"

namespace merutilm::rff2 {
    class RenderGraph1 final : public vkh::RenderPassGraphGenerator {

        vkh::RenderPassAttachment *resultAttachment = nullptr;
        vkh::RenderPassAttachment *tempAttachment = nullptr;
        vkh::RenderPassAttachment *depthAttachment = nullptr;

    public:
        GPC3DFractal *fractal3d = nullptr;
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
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_IMAGE_SECONDARY), VkClearValue{.color = {0, 0, 0, 1}});
            depthAttachment = &appendAttachment(
                    VkAttachmentDescription{
                            .flags = 0,
                            .format = wc.getSharedImageContext()
                                              .getImageContextMF(MF_MAIN_RENDER_IMAGE_DEPTH)[0]
                                              .imageFormat,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                    },
                    wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_IMAGE_DEPTH), VkClearValue{.depthStencil = {1, 0}});
            resultAttachment = &appendAttachment(
                    VkAttachmentDescription{
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
                    wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_IMAGE_PRIMARY), VkClearValue{.color = {0, 0, 0, 1}});
        }


        void configurePipelines() override {


            vkh::GraphicsPipelineNode *fractal3dNode = registerPipeline<GPC3DFractal>(
                    &fractal3d, {},
                    {{tempAttachment, RendererUtils::COLOR_REF_INFO, RendererUtils::INPUT_READ_DEPENDENCY,
                      RendererUtils::INPUT_REF_INFO},
                     {depthAttachment, RendererUtils::DEPTH_REF_INFO, std::nullopt, std::nullopt}},
                    RendererUtils::DEFAULT_DESC_PICKER);


            registerPipeline<GPCColor>(&color, {fractal3dNode},
                                       {resultAttachment, RendererUtils::COLOR_REF_INFO, std::nullopt, std::nullopt},
                                       RendererUtils::DEFAULT_DESC_PICKER);
        }
    };
} // namespace merutilm::rff2
