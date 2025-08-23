//
// Created by Merutilm on 2025-07-14.
//

#include "Framebuffer.hpp"

#include "../util/BufferImageUtils.hpp"

namespace merutilm::vkh {
    Framebuffer::Framebuffer(const CoreRef core, const RenderPass &renderPass,
                             const VkExtent2D extent) : CoreHandler(core), renderPass(renderPass), extent(extent) {
        Framebuffer::init();
    }

    Framebuffer::~Framebuffer() {
        Framebuffer::destroy();
    }

    void Framebuffer::init() {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const auto [width, height] = extent;
        framebuffer.resize(maxFramesInFlight);
        auto & attachments = renderPass.getManager().getAttachments();

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            auto attachmentImageViews = std::vector<VkImageView>(attachments.size());
            std::ranges::transform(attachments, attachmentImageViews.begin(), [i](const RenderPassAttachment &v) {
                return v.imageContext[i].imageView;
            });



            const VkFramebufferCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .renderPass = renderPass.getRenderPassHandle(),
                .attachmentCount = static_cast<uint32_t>(attachmentImageViews.size()),
                .pAttachments = attachmentImageViews.data(),
                .width = width,
                .height = height,
                .layers = 1
            };


            if (vkCreateFramebuffer(core.getLogicalDevice().getLogicalDeviceHandle(), &createInfo, nullptr,
                                    &framebuffer[i]) != VK_SUCCESS) {
                throw exception_init("Failed to create framebuffer");
            }
        }
    }

    void Framebuffer::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            vkDestroyFramebuffer(device, framebuffer[i], nullptr);
        }
    }


}
