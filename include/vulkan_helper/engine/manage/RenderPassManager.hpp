//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/engine/wrapped/RenderPassAttachment.hpp>
#include <vulkan_helper/engine/wrapped/RenderPassAttachmentType.hpp>

namespace merutilm::vkh {
    struct RenderPassManager {
        std::vector<RenderPassAttachment> attachments = {};
        std::vector<std::vector<uint32_t> > preserveIndices = {};
        std::vector<std::unordered_map<RenderPassAttachmentType, std::vector<VkAttachmentReference> > >attachmentReferences{};
        std::vector<VkSubpassDependency> subpassDependencies = {};
        uint32_t subpassCount = 0;

        explicit RenderPassManager();

        ~RenderPassManager();

        RenderPassManager(const RenderPassManager &) = delete;

        RenderPassManager &operator=(const RenderPassManager &) = delete;

        RenderPassManager(RenderPassManager &&) noexcept = default;

        RenderPassManager &operator=(RenderPassManager &&) = delete;


        void setPreserved(uint32_t attachmentIndex);


        void unsetPreserved(uint32_t attachmentIndex);


        void appendSubpass(uint32_t subpassIndexExpected);


        void appendReference(uint32_t attachmentIndex,
                             RenderPassAttachmentType attachmentType, VkImageLayout layoutToUse);

        void appendAttachment(uint32_t attachmentIndexExpected,
                              const VkAttachmentDescription &attachmentDescription,
                              const MultiframeImageContext &imageContext);


        void appendDependency(const VkSubpassDependency &subpassDependency);
    };

}
