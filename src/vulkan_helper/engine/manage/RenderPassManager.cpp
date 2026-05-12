//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/manage/RenderPassManager.hpp>

namespace merutilm::vkh {
    RenderPassManager::RenderPassManager() = default;

    RenderPassManager::~RenderPassManager() = default;

    void RenderPassManager::setPreserved(const uint32_t attachmentIndex) {
        preserveIndices.back().emplace_back(attachmentIndex);
    }

    void RenderPassManager::unsetPreserved(const uint32_t attachmentIndex) {
        auto &indices = preserveIndices.back();
        const auto it = std::ranges::find(indices, attachmentIndex);
        if (it == indices.end()) {
            throw exception_invalid_args("given attachment is not preserved");
        }
        indices.erase(it);
    }

    void RenderPassManager::appendSubpass(const uint32_t subpassIndexExpected) {
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

    void RenderPassManager::appendReference(const uint32_t attachmentIndex,
        const RenderPassAttachmentType attachmentType, const VkImageLayout layoutToUse) {
        using enum RenderPassAttachmentType;
        auto &ref = this->attachmentReferences.back()[attachmentType];
        ref.emplace_back(VkAttachmentReference{
            .attachment = attachmentIndex,
            .layout = layoutToUse,
        });
    }

    void RenderPassManager::appendAttachment(const uint32_t attachmentIndexExpected,
        const VkAttachmentDescription &attachmentDescription, const MultiframeImageContext &imageContext) {
        attachments.emplace_back(attachmentDescription, imageContext);
        safe_array::check_index_equal(attachmentIndexExpected, static_cast<uint32_t>(attachments.size() - 1),
                                      "Attachment Index");
    }

    void RenderPassManager::appendDependency(const VkSubpassDependency &subpassDependency) {
        subpassDependencies.push_back(subpassDependency);
    }
}
