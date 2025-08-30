//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include "../../vulkan_helper/configurator/RenderContextConfigurator.hpp"
#include "../../vulkan_helper/util/ImageContextUtils.hpp"

namespace merutilm::rff2 {
    struct RCCFirst final : public vkh::RenderContextConfiguratorAbstract {
        static constexpr uint32_t CONTEXT_INDEX = 0;

        static constexpr uint32_t SUBPASS_ITERATION_INDEX = 0;
        static constexpr uint32_t SUBPASS_STRIPE_INDEX = 1;
        static constexpr uint32_t SUBPASS_SLOPE_INDEX = 2;
        static constexpr uint32_t SUBPASS_COLOR_INDEX = 3;

        static constexpr uint32_t RESULT_COLOR_ATTACHMENT_INDEX = 0;
        static constexpr uint32_t TEMP_COLOR_ATTACHMENT_INDEX = 1;

        static constexpr uint32_t RESULT_IMAGE_CONTEXT = 0;
        static constexpr uint32_t TEMP_IMAGE_CONTEXT = 1;


        using RenderContextConfiguratorAbstract::RenderContextConfiguratorAbstract;

        void configureImageContext(const VkExtent2D &extent) override {
            const auto [width, height] = extent;

            appendStoredImageContext(RESULT_IMAGE_CONTEXT, vkh::ImageContext::createMultiframeContext(
                                         core, vkh::ImageInitInfo{
                                             .imageType = VK_IMAGE_TYPE_2D,
                                             .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                                             .imageFormat = VK_FORMAT_R8G8B8A8_UNORM,
                                             .extent = {width, height, 1},
                                             .useMipmap = false,
                                             .arrayLayers = 1,
                                             .samples = VK_SAMPLE_COUNT_1_BIT,
                                             .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                                             .usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                                      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                             .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                             .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                                         }));

            appendStoredImageContext(TEMP_IMAGE_CONTEXT, vkh::ImageContext::createMultiframeContext(
                                         core, vkh::ImageInitInfo{
                                             .imageType = VK_IMAGE_TYPE_2D,
                                             .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                                             .imageFormat = VK_FORMAT_R8G8B8A8_UNORM,
                                             .extent = {width, height, 1},
                                             .useMipmap = false,
                                             .arrayLayers = 1,
                                             .samples = VK_SAMPLE_COUNT_1_BIT,
                                             .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                                             .usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                                      VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                                             .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                             .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                                         }));
        }

        void configureRenderContext(vkh::RenderPassManagerRef rpm) override {
            rpm.appendAttachment(RESULT_COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = contexts[RESULT_IMAGE_CONTEXT][0].imageFormat,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 }, contexts[RESULT_IMAGE_CONTEXT]);

            rpm.appendAttachment(TEMP_COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = contexts[TEMP_IMAGE_CONTEXT][0].imageFormat,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 }, contexts[TEMP_IMAGE_CONTEXT]);



            rpm.appendSubpass(SUBPASS_ITERATION_INDEX);
            rpm.appendReference(TEMP_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            rpm.appendSubpass(SUBPASS_STRIPE_INDEX);
            rpm.appendReference(TEMP_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::INPUT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            rpm.appendSubpass(SUBPASS_SLOPE_INDEX);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::INPUT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            rpm.appendReference(TEMP_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

            rpm.appendSubpass(SUBPASS_COLOR_INDEX);
            rpm.appendReference(TEMP_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::INPUT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
            rpm.appendReference(RESULT_COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);


            rpm.appendDependency({
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = SUBPASS_ITERATION_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dependencyFlags = 0
            });
            rpm.appendDependency({
                .srcSubpass = SUBPASS_ITERATION_INDEX,
                .dstSubpass = SUBPASS_STRIPE_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
                .dependencyFlags = 0
            });
            rpm.appendDependency({
                .srcSubpass = SUBPASS_STRIPE_INDEX,
                .dstSubpass = SUBPASS_SLOPE_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
                .dependencyFlags = 0
            });
            rpm.appendDependency({
                .srcSubpass = SUBPASS_SLOPE_INDEX,
                .dstSubpass = SUBPASS_COLOR_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
                .dependencyFlags = 0
            });
            rpm.appendDependency({
                .srcSubpass = SUBPASS_COLOR_INDEX,
                .dstSubpass = VK_SUBPASS_EXTERNAL,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = 0,
                .dependencyFlags = 0
            });
        }
    };
}
