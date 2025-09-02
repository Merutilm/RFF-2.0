//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include "../core/vkh_core.hpp"
#include "../struct/RenderPassAttachment.hpp"
#include "../struct/RenderPassAttachmentType.hpp"

namespace merutilm::vkh {
    struct RenderPassManagerImpl {
        std::vector<RenderPassAttachment> attachments = {};
        std::vector<std::vector<uint32_t> > preserveIndices = {};
        std::vector<std::unordered_map<RenderPassAttachmentType, std::vector<VkAttachmentReference> > >
        attachmentReferences{};
        std::vector<VkSubpassDependency> subpassDependencies = {};
        uint32_t subpassCount = 0;

        explicit RenderPassManagerImpl() = default;

        ~RenderPassManagerImpl() = default;

        RenderPassManagerImpl(const RenderPassManagerImpl &) = delete;

        RenderPassManagerImpl &operator=(const RenderPassManagerImpl &) = delete;

        RenderPassManagerImpl(RenderPassManagerImpl &&) = delete;

        RenderPassManagerImpl &operator=(RenderPassManagerImpl &&) = delete;


        void setPreserved(const uint32_t attachmentIndex) {
            preserveIndices.back().emplace_back(attachmentIndex);
        }


        void unsetPreserved(const uint32_t attachmentIndex) {
            auto &indices = preserveIndices.back();
            const auto it = std::ranges::find(indices, attachmentIndex);
            if (it == indices.end()) {
                throw exception_invalid_args("given attachment is not preserved");
            }
            indices.erase(it);
        }


        void appendSubpass(const uint32_t subpassIndexExpected) {
            using enum RenderPassAttachmentType;
            attachmentReferences.emplace_back();
            attachmentReferences.back()[INPUT];
            attachmentReferences.back()[COLOR];
            attachmentReferences.back()[RESOLVE];
            attachmentReferences.back()[DEPTH_STENCIL];
            preserveIndices.emplace_back();
            safe_array::check_index_equal(subpassIndexExpected, subpassCount, "Subpass Index");
            ++subpassCount;
        }


        void appendReference(const uint32_t attachmentIndex,
                             const RenderPassAttachmentType attachmentType, const VkImageLayout layoutToUse) {
            using enum RenderPassAttachmentType;
            auto &ref = this->attachmentReferences.back()[attachmentType];
            ref.emplace_back(VkAttachmentReference{
                .attachment = attachmentIndex,
                .layout = layoutToUse,
            });
        }

        void appendAttachment(const uint32_t attachmentIndexExpected,
                              const VkAttachmentDescription &attachmentDescription,
                              const MultiframeImageContext &imageContext) {
            attachments.emplace_back(attachmentDescription, imageContext);
            safe_array::check_index_equal(attachmentIndexExpected, static_cast<uint32_t>(attachments.size() - 1),
                                              "Attachment Index");
        }


        void appendDependency(const VkSubpassDependency &subpassDependency) {
            subpassDependencies.push_back(subpassDependency);
        }
    };
    using RenderPassManager = std::unique_ptr<RenderPassManagerImpl>;
    using RenderPassManagerPtr = RenderPassManagerImpl *;
    using RenderPassManagerRef = RenderPassManagerImpl &;
}
