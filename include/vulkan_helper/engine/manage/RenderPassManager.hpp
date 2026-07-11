//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/engine/wrapped/RenderPassAttachment.hpp>
#include <vulkan_helper/engine/wrapped/RenderPassAttachmentType.hpp>

#include "vulkan_helper/engine/wrapped/RenderPassAttachmentReferenceDetail.hpp"

namespace merutilm::vkh {
    struct RenderPassManager {
        std::vector<std::unique_ptr<RenderPassAttachment>> attachments = {};
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


        void setPreserved(const RenderPassAttachment &attachment, uint32_t subpassIndex = UINT32_MAX);

        void appendSubpass();


        void appendReference(const RenderPassAttachment * targetAttachment, const RenderPassAttachmentReference &attachmentReference,
                             uint32_t subpassIndex = UINT32_MAX);

        RenderPassAttachment &appendAttachment(const VkAttachmentDescription &attachmentDescription,
                                               const MultiframeImageContext &imageContext);


        void appendDependency(const VkSubpassDependency &subpassDependency);

        [[nodiscard]] uint32_t subpassIndexToIndex(uint32_t subpassIndex) const;
    };

}
