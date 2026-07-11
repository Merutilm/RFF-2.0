//
// Created by Merutilm on 2025-08-29.
//

#pragma once
#include "GPCDownsampleForBlur.hpp"
#include "SharedImageContextIndices.hpp"
#include "vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp"

namespace merutilm::rff2 {
    class RCCDownsampleForBlur final : public vkh::RenderPassGraphGenerator {

        vkh::RenderPassAttachment *resultAttachment;

    public:
        enum class DescIndexer : uint8_t { FOG, BLOOM };

        GPCDownsampleForBlur *downsample;
        DescIndexer descIndexer;

        using RenderPassGraphGenerator::RenderPassGraphGenerator;

    protected:
        void configureAttachments() override {
            using namespace SharedImageContextIndices;
            resultAttachment = &appendAttachment(
                    {
                            .flags = 0,
                            .format = wc.getSharedImageContext()
                                              .getImageContextMF(MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY)[0]
                                              .imageFormat,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_GENERAL,
                    },
                    wc.getSharedImageContext().getImageContextMF(MF_MAIN_RENDER_DOWNSAMPLED_IMAGE_PRIMARY));
        }
        void configurePipelines() override {

            registerPipeline<GPCDownsampleForBlur>(
                    &downsample, {},
                    {resultAttachment,
                     {vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
                     vkh::SubpassDependency::none(),
                     vkh::RenderPassAttachmentReference::none()},
                    [this] {
                        return vkh::DescIndexPicker{descIndexer == DescIndexer::FOG
                                                            ? GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_FOG
                                                            : GPCDownsampleForBlur::DESC_INDEX_RESAMPLE_IMAGE_BLOOM};
                    });
        }
    };
} // namespace merutilm::rff2
