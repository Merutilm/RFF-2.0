//
// Created by Merutilm on 2025-07-14.
//

#include "Framebuffer.hpp"

namespace merutilm::vkh {
    FramebufferImpl::FramebufferImpl(CoreRef core, RenderPassRef renderPass,
                             const VkExtent2D extent) : CoreHandler(core), renderPass(renderPass), extent(extent) {
        FramebufferImpl::init();
    }

    FramebufferImpl::~FramebufferImpl() {
        FramebufferImpl::destroy();
    }

    void FramebufferImpl::init() {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const auto [width, height] = extent;
        framebuffer.resize(maxFramesInFlight);
        auto & attachments = renderPass.getAttachments();

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            auto attachmentWriteImageViews = std::vector<VkImageView>(attachments.size());
            std::ranges::transform(attachments, attachmentWriteImageViews.begin(), [i](const RenderPassAttachment &v) {
                return v.imageContext[i].imageView;
            });



            const VkFramebufferCreateInfo createInfo = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .renderPass = renderPass.getRenderPassHandle(),
                .attachmentCount = static_cast<uint32_t>(attachmentWriteImageViews.size()),
                .pAttachments = attachmentWriteImageViews.data(),
                .width = width,
                .height = height,
                .layers = 1
            };


            if (allocator::invoke(vkCreateFramebuffer, core.getLogicalDevice().getLogicalDeviceHandle(), &createInfo, nullptr,
                                    &framebuffer[i]) != VK_SUCCESS) {
                throw exception_init("Failed to create framebuffer");
            }
        }
    }

    void FramebufferImpl::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            allocator::invoke(vkDestroyFramebuffer, device, framebuffer[i], nullptr);
        }
    }


}
