//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/manage/RenderPassManager.hpp>

#include "vulkan_helper/engine/wrapped/RenderPassAttachmentReferenceDetail.hpp"

namespace merutilm::vkh {
    RenderPassManager::RenderPassManager() = default;

    RenderPassManager::~RenderPassManager() = default;

    void RenderPassManager::setPreserved(const RenderPassAttachment &attachment, const uint32_t subpassIndex) {
        preserveIndices[subpassIndexToIndex(subpassIndex)].emplace_back(attachment.attachmentIndex);
    }


    void RenderPassManager::appendSubpass() {
        using enum RenderPassAttachmentType;
        attachmentReferences.emplace_back();
        attachmentReferences.back()[INPUT];
        attachmentReferences.back()[COLOR];
        attachmentReferences.back()[RESOLVE];
        attachmentReferences.back()[DEPTH_STENCIL];
        preserveIndices.emplace_back();
        ++subpassCount;
    }

    void RenderPassManager::appendReference(const RenderPassAttachment * targetAttachment, const RenderPassAttachmentReference &attachmentReference, const uint32_t subpassIndex) {
        using enum RenderPassAttachmentType;
        auto &ref = this->attachmentReferences[subpassIndexToIndex(subpassIndex)][attachmentReference.attachmentType];
        ref.emplace_back(VkAttachmentReference{
            .attachment = targetAttachment->attachmentIndex,
            .layout = attachmentReference.layoutToUse,
        });
    }

    RenderPassAttachment &RenderPassManager::appendAttachment(const VkAttachmentDescription &attachmentDescription, const MultiframeImageContext &imageContext) {
        return *attachments.emplace_back(std::make_unique<RenderPassAttachment>(attachments.size(), attachmentDescription, imageContext));
    }

    void RenderPassManager::appendDependency(const VkSubpassDependency &subpassDependency) {
        subpassDependencies.push_back(subpassDependency);
    }

    uint32_t RenderPassManager::subpassIndexToIndex(const uint32_t subpassIndex) const {
        return subpassIndex == UINT32_MAX ? subpassCount - 1 : subpassIndex;
    }
} // namespace merutilm::vkh
