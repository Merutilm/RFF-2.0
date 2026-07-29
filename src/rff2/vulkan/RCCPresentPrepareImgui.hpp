//
// Created by Merutilm on 2025-08-28.
//

#pragma once
#include "GPCSmoothZoom.hpp"
#include "vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp"

namespace merutilm::rff2 {
    class RCCPresentPrepareImgui final : public vkh::RenderPassGraphGenerator {

        vkh::RenderPassAttachment *swapchainAttachment;

    public:
        GPCSmoothZoom *smoothZoom;

        using RenderPassGraphGenerator::RenderPassGraphGenerator;

    protected:
        void configureAttachments() override {

            swapchainAttachment =
                    &appendAttachment({.flags = 0,
                                       .format = wc.core.getPhysicalDeviceLoader().getPrimarySurfaceFormat(),
                                       .samples = VK_SAMPLE_COUNT_1_BIT,
                                       .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                       .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                       .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                       .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                       .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                       .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}, //imgui rendering requires color attachment for swapchain
                                      swapchainImageContextGetter());
        }

        void configurePipelines() override {

            registerPipeline<GPCSmoothZoom>(
                    &smoothZoom, {},
                    {swapchainAttachment,
                        RendererUtils::COLOR_REF_INFO,
                     RendererUtils::INPUT_READ_DEPENDENCY,
                     RendererUtils::INPUT_REF_INFO},
                    RendererUtils::DEFAULT_DESC_PICKER);

        }
    };
} // namespace merutilm::rff2
