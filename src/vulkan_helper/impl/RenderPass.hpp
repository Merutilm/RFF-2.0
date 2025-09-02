//
// Created by Merutilm on 2025-07-11.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "../manage/RenderPassManager.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class RenderPassImpl final : public CoreHandler {
        const std::vector<RenderPassAttachment> attachments;
        const std::vector<std::vector<uint32_t> > preserveIndices;
        const std::vector<std::unordered_map<RenderPassAttachmentType, std::vector<VkAttachmentReference> > >
        attachmentReferences;
        const std::vector<VkSubpassDependency> subpassDependencies;
        const uint32_t subpassCount;
        VkRenderPass renderPass = VK_NULL_HANDLE;

    public:
        explicit RenderPassImpl(CoreRef core, RenderPassManager &&manager);

        ~RenderPassImpl() override;

        RenderPassImpl(const RenderPassImpl &) = delete;

        RenderPassImpl &operator=(const RenderPassImpl &) = delete;

        RenderPassImpl(RenderPassImpl &&) = delete;

        RenderPassImpl &operator=(RenderPassImpl &&) = delete;


        [[nodiscard]] uint32_t getPreserveIndicesCount(const uint32_t subpassIndex) const {
            return static_cast<uint32_t>(preserveIndices[subpassIndex].size());
        };

        [[nodiscard]] const uint32_t *getPreserveIndices(const uint32_t subpassIndex) const {
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


        [[nodiscard]] VkRenderPass getRenderPassHandle() const { return renderPass; }

    private:
        void init() override;

        void destroy() override;
    };

    using RenderPass = std::unique_ptr<RenderPassImpl>;
    using RenderPassPtr = RenderPassImpl *;
    using RenderPassRef = RenderPassImpl &;
}
