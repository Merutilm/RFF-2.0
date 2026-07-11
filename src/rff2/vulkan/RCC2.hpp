//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include <vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp>

#include "GPCColor.hpp"
#include "GPCSlope.hpp"
#include "SharedImageContextIndices.hpp"

namespace merutilm::rff2 {
    class RCC2 final : public vkh::RenderPassGraphGenerator {

        vkh::RenderPassAttachment *resultAttachment;
        vkh::RenderPassAttachment *tempAttachment;

    public:
        GPCSlope *slope;
        GPCColor *color;

        using RenderPassGraphGenerator::RenderPassGraphGenerator;


    protected:
        void configureAttachments() override {
            using namespace SharedImageContextIndices;
            resultAttachment = &appendAttachment(
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

            tempAttachment = &appendAttachment(
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

            vkh::GraphicsPipelineNode *slopeNode = registerPipeline<GPCSlope>(
                    &slope, {},
                    {tempAttachment,
                     {vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                     {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                      VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
                      VK_DEPENDENCY_BY_REGION_BIT},
                     {vkh::RenderPassAttachmentType::INPUT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}},
                    [] { return vkh::DescIndexPicker{}; });

            registerPipeline<GPCColor>(&color, {slopeNode},
                                   {resultAttachment,
                                    {vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                                    vkh::SubpassDependency::none(),
                                    vkh::RenderPassAttachmentReference::none()},
                                   [] { return vkh::DescIndexPicker{}; });
        }
    };
} // namespace merutilm::rff2
