//
// Created by Merutilm on 2025-07-11.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/engine/manage/RenderPassManager.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class RenderPass final : public CoreHandler {
        const std::vector<std::unique_ptr<RenderPassAttachment>> &attachments;
        const std::vector<std::vector<uint32_t>> &preserveIndices;
        const std::vector<std::unordered_map<RenderPassAttachmentType, std::vector<VkAttachmentReference>>>
                &attachmentReferences;
        const std::vector<VkSubpassDependency> &subpassDependencies;
        const uint32_t subpassCount;
        VkRenderPass renderPass = VK_NULL_HANDLE;

    public:
        explicit RenderPass(Core &core, RenderPassManager &manager);

        ~RenderPass() override;

        RenderPass(const RenderPass &) = delete;

        RenderPass &operator=(const RenderPass &) = delete;

        RenderPass(RenderPass &&) = delete;

        RenderPass &operator=(RenderPass &&) = delete;


        [[nodiscard]] const std::vector<std::unique_ptr<RenderPassAttachment>> &getAttachments() const {
            return attachments;
        }

        [[nodiscard]] uint32_t getSubpassCount() const { return subpassCount; }


        [[nodiscard]] const std::vector<VkAttachmentReference> &
        getAttachmentReferences(const uint32_t subpassIndex, const RenderPassAttachmentType attachmentType) const {
            return attachmentReferences[subpassIndex].at(attachmentType);
        }

        [[nodiscard]] const std::vector<VkSubpassDependency> &getSubpassDependencies() const {
            return subpassDependencies;
        }


        [[nodiscard]] VkRenderPass getRenderPassHandle() const { return renderPass; }

    protected:
        void init() override;

        void cleanup() override;
    };


} // namespace merutilm::vkh
