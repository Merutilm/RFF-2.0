//
// Created by Merutilm on 2025-08-30.
//

#pragma once
#include "GPCBloom.hpp"
#include "GPCLinearInterpolation.hpp"
#include "SharedImageContextIndices.hpp"
#include "vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp"


namespace merutilm::rff2 {
    class RenderGraph4 final : public vkh::RenderPassGraphGenerator {

        vkh::RenderPassAttachment *tempAttachment;
        vkh::RenderPassAttachment *resultAttachment;

    public:
        GPCBloom *bloom;
        GPCLinearInterpolation *linearInterpolation;

        using RenderPassGraphGenerator::RenderPassGraphGenerator;

    protected:
        void configureAttachments() override {
            using namespace SharedImageContextIndices;
            tempAttachment = &appendAttachment(
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
            vkh::GraphicsPipelineNode *bloomNode =
                    registerPipeline(&bloom, {},
                                     {tempAttachment, RendererUtils::COLOR_REF_INFO,
                                      RendererUtils::SAMPLER_READ_DEPENDENCY, RendererUtils::INPUT_REF_INFO},
                                     RendererUtils::DEFAULT_DESC_PICKER);

            registerPipeline(&linearInterpolation, {bloomNode},
                             {resultAttachment, RendererUtils::COLOR_REF_INFO, std::nullopt, std::nullopt},
                             RendererUtils::DEFAULT_DESC_PICKER);
        }
    };
} // namespace merutilm::rff2
