//
// Created by Merutilm on 2025-07-22.
//

#pragma once
#include "../../vulkan_helper/configurator/RenderContextConfigurator.hpp"

namespace merutilm::rff2 {
    struct BasicRenderContextConfigurator final : vkh::RenderContextConfigurator {
        static constexpr uint32_t SUBPASS_PRIMARY_INDEX = 0;
        static constexpr uint32_t SUBPASS_SECONDARY_INDEX = 1;

        static constexpr uint32_t COLOR_ATTACHMENT_INDEX = 0;
        static constexpr uint32_t PRESENT_ATTACHMENT_INDEX = 1;

        static constexpr uint32_t PRIMARY_SUBPASS_RESULT_IMAGE = 0;

        explicit BasicRenderContextConfigurator(const vkh::Core &core,
                                                const vkh::Swapchain &swapchain) : RenderContextConfigurator(
            core, swapchain) {
        }

        void configureImageContext() override {
            const auto [width, height] = swapchain.populateSwapchainExtent();

            appendStoredImageContext(PRIMARY_SUBPASS_RESULT_IMAGE, vkh::ImageContext::init(core, vkh::ImageInitInfo{
                                         .imageType = VK_IMAGE_TYPE_2D,
                                         .imageViewType = VK_IMAGE_VIEW_TYPE_2D,
                                         .imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
                                         .extent = {width, height, 1},
                                         .mipLevels = 1,
                                         .arrayLayers = 1,
                                         .samples = VK_SAMPLE_COUNT_1_BIT,
                                         .imageTiling = VK_IMAGE_TILING_OPTIMAL,
                                         .usage = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT |
                                                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                                                  VK_IMAGE_USAGE_SAMPLED_BIT,
                                         .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                         .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                                     }));
        }

        void configureRenderContext(vkh::RenderPassManager &rpm) override {
            rpm.appendAttachment(COLOR_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = VK_FORMAT_R8G8B8A8_SRGB,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                 }, contexts[PRIMARY_SUBPASS_RESULT_IMAGE]);
            rpm.appendAttachment(PRESENT_ATTACHMENT_INDEX, {
                                     .flags = 0,
                                     .format = VK_FORMAT_R8G8B8A8_SRGB,
                                     .samples = VK_SAMPLE_COUNT_1_BIT,
                                     .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                     .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                     .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                                 }, vkh::ImageContext::fromSwapchain(
                                     swapchain, core.getPhysicalDevice().getMaxFramesInFlight()));


            rpm.appendSubpass(SUBPASS_PRIMARY_INDEX);
            rpm.appendReference(COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR);
            rpm.appendSubpass(SUBPASS_SECONDARY_INDEX);
            rpm.appendReference(COLOR_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::INPUT);
            rpm.appendReference(PRESENT_ATTACHMENT_INDEX, vkh::RenderPassAttachmentType::COLOR);

            rpm.appendDependency({
                .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = SUBPASS_PRIMARY_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dependencyFlags = 0
            });
            rpm.appendDependency({
                .srcSubpass = SUBPASS_PRIMARY_INDEX,
                .dstSubpass = SUBPASS_SECONDARY_INDEX,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                .dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
                .dependencyFlags = 0
            });
            rpm.appendDependency({
                .srcSubpass = SUBPASS_SECONDARY_INDEX,
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
