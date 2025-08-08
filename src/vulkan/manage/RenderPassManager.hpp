//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "../exception/exception.hpp"
#include "../struct/RenderPassAttachment.hpp"
#include "../struct/RenderPassAttachmentType.hpp"
#include "../util/IndexChecker.hpp"

namespace merutilm::mvk {
    class RenderPassManager {
        std::vector<RenderPassAttachment> attachments = {};
        std::vector<std::vector<uint32_t> > preserveIndices = {};
        std::vector<std::unordered_map<RenderPassAttachmentType, std::vector<VkAttachmentReference> > >
        attachmentReferences{};
        std::vector<VkSubpassDependency> subpassDependencies = {};
        uint32_t subpassCount = 0;

    public:
        explicit RenderPassManager() = default;

        ~RenderPassManager() = default;

        RenderPassManager(const RenderPassManager &) = delete;

        RenderPassManager &operator=(const RenderPassManager &) = delete;

        RenderPassManager(RenderPassManager &&) = delete;

        RenderPassManager &operator=(RenderPassManager &&) = delete;

        [[nodiscard]] uint32_t getPreserveIndicesCount(const uint32_t subpassIndex) const {
            return static_cast<uint32_t>(preserveIndices[subpassIndex].size());
        };

        [[nodiscard]] uint32_t *getPreserveIndices(const uint32_t subpassIndex) {
            return preserveIndices[subpassIndex].data();
        }

        [[nodiscard]] const std::vector<RenderPassAttachment> &getAttachments() const {
            return attachments;
        }


        [[nodiscard]] uint32_t getSubpassCount() const { return subpassCount; }


        [[nodiscard]] const std::vector<VkAttachmentReference> &getAttachmentReferences(
            const uint32_t subpassIndex, const RenderPassAttachmentType attachmentType) const {
            return attachmentReferences[subpassIndex].at(attachmentType);
        }

        [[nodiscard]] const std::vector<VkSubpassDependency> &getSubpassDependencies() const {
            return subpassDependencies;
        }


        void setPreserved(uint32_t attachmentIndex);

        void unsetPreserved(uint32_t attachmentIndex);

        void appendSubpass(uint32_t subpassIndexExpected);

        void appendReference(uint32_t attachmentIndex, RenderPassAttachmentType attachmentType);

        void appendAttachment(uint32_t attachmentIndexExpected,
                              const VkAttachmentDescription &attachmentDescription,
                              const MultiframeImageContext &imageContext);


        void appendDependency(const VkSubpassDependency &subpassDependency);

    };


    inline void RenderPassManager::setPreserved(const uint32_t attachmentIndex) {
        preserveIndices.back().emplace_back(attachmentIndex);
    }


    inline void RenderPassManager::unsetPreserved(const uint32_t attachmentIndex) {
        auto &indices = preserveIndices.back();
        const auto it = std::ranges::find(indices, attachmentIndex);
        if (it == indices.end()) {
            throw exception_invalid_args("given attachment is not preserved");
        }
        indices.erase(it);
    }


    inline void RenderPassManager::appendSubpass(const uint32_t subpassIndexExpected) {
        using enum RenderPassAttachmentType;
        attachmentReferences.emplace_back();
        attachmentReferences.back()[INPUT];
        attachmentReferences.back()[COLOR];
        attachmentReferences.back()[RESOLVE];
        attachmentReferences.back()[DEPTH_STENCIL];
        preserveIndices.emplace_back();
        IndexChecker::checkIndexEqual(subpassIndexExpected, subpassCount, "Subpass Index");
        ++subpassCount;
    }

    inline void RenderPassManager::appendReference(const uint32_t attachmentIndex,
                                                   const RenderPassAttachmentType attachmentType) {
        using enum RenderPassAttachmentType;

        auto &ref = this->attachmentReferences.back()[attachmentType];
        VkAttachmentReference attachmentReference = {};

        switch (attachmentType) {
            case INPUT: {
                attachmentReference = {
                    .attachment = attachmentIndex,
                    .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                };
                break;
            }
            case COLOR:
            case RESOLVE: {
                attachmentReference = {
                    .attachment = attachmentIndex,
                    .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                };
                break;
            }
            case DEPTH_STENCIL: {
                if (!ref.empty()) {
                    throw exception_invalid_args("Depth Stencil attachment has already been set");
                }
                attachmentReference = {
                    .attachment = attachmentIndex,
                    .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                };
                break;
            }
        }
        ref.emplace_back(attachmentReference);
    }


    inline void RenderPassManager::appendAttachment(const uint32_t attachmentIndexExpected,
                                                    const VkAttachmentDescription &attachmentDescription,
                                                    const MultiframeImageContext &imageContext) {
        attachments.emplace_back(attachmentDescription, imageContext);
        IndexChecker::checkIndexEqual(attachmentIndexExpected, static_cast<uint32_t>(attachments.size() - 1), "Attachment Index");
    }


    inline void RenderPassManager::appendDependency(const VkSubpassDependency &subpassDependency) {
        subpassDependencies.push_back(subpassDependency);
    }
}
