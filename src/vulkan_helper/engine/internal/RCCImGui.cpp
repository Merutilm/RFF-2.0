//
// Created by Merutilm on 7/7/26.
//

#ifdef VKH_USE_IMGUI
#include <vulkan_helper/engine/internal/RCCImGui.hpp>

namespace merutilm::vkh {



    void RCCImGui::configureAttachments() {
        swapchainImageAttachment = &rpm.appendAttachment({.flags = 0,
                                       .format = engine.getCore().getPhysicalDeviceLoader().getPrimarySurfaceFormat(),
                                       .samples = VK_SAMPLE_COUNT_1_BIT,
                                       .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
                                       .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                                       .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                                       .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                                       .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
                                      swapchainImageContextGetter());
    }

    void RCCImGui::configurePipelines() {


        rpm.appendSubpass();
        rpm.appendReference(swapchainImageAttachment, {
                .attachmentType = RenderPassAttachmentType::COLOR,
                .layoutToUse = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        });
    }


} // namespace merutilm::vkh
#endif
