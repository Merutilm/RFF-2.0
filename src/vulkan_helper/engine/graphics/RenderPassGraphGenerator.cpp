//
// Created by Merutilm on 7/9/26.
//


#include <vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp>
#include <queue>
#include <ranges>

namespace merutilm::vkh {

    RenderPassAttachment &
    RenderPassGraphGenerator::appendAttachment(const VkAttachmentDescription &attachmentDescription,
                                               const MultiframeImageContext &imageContext) {
        return rpm.appendAttachment(attachmentDescription, imageContext);
    }
    void RenderPassGraphGenerator::createPipelines(RenderPass *rp) const {
        for (auto &node: pipelines) {
            node->getPipelineConfigurator().configure(rp, node->getSubpass());
        }
    }

    void RenderPassGraphGenerator::generate() {
        if (pipelines.empty())
            return;

        // auto-generate render pass graph. (Topological sort)
        // find subpass 0
        std::unordered_map<const GraphicsPipelineNode *, uint32_t> referenceCount;
        std::queue<const GraphicsPipelineNode *> queue;

        for (auto &pipeline: pipelines) {
            if (pipeline->getSubpass() == 0) {
                assert(pipeline->getDepends().empty()); // it should not be happened

                queue.push(pipeline.get());
            }
            referenceCount.emplace(pipeline.get(), pipeline->getDepends().size());
        }

        std::unordered_map<const RenderPassAttachment *, std::unordered_set<uint32_t>> usedSubpasses;
        std::unordered_map<uint32_t, std::unordered_map<uint32_t, VkSubpassDependency>> generatedDependencies;

        while (!queue.empty()) {
            const GraphicsPipelineNode *node = queue.front();
            queue.pop();
            processNode(usedSubpasses, generatedDependencies, node);
            for (const auto next: node->getNext()) {
                --referenceCount[next];

                if (referenceCount[next] == 0) {
                    queue.push(next);
                }
            }
        }

        for (auto &[attachment, subpasses]: usedSubpasses) {
            if (subpasses.empty())
                continue;

            std::vector<uint32_t> sorted{subpasses.begin(), subpasses.end()};
            std::ranges::sort(sorted);
            uint32_t prevSp = sorted[0];
            for (size_t i = 1; i < sorted.size(); ++i) {
                const uint32_t sp = sorted[i];

                for (uint32_t j = prevSp + 1; j <= sp - 1; ++j) {
                    rpm.setPreserved(*attachment, j);
                }
                prevSp = sp;
            }
        }

        for (const auto &val: generatedDependencies | std::views::values) {
            for (const auto &val2: val | std::views::values) {
                rpm.appendDependency(val2);
            }
        }

        std::ranges::sort(pipelines, [](const std::unique_ptr<GraphicsPipelineNode> &a,
                                        const std::unique_ptr<GraphicsPipelineNode> &b) {
            return a->getSubpass() < b->getSubpass();
        });
    }


    void RenderPassGraphGenerator::processNode(
            std::unordered_map<const RenderPassAttachment *, std::unordered_set<uint32_t>> &usedSubpasses,
            std::unordered_map<uint32_t, std::unordered_map<uint32_t, VkSubpassDependency>> &generatedDependencies,
            const GraphicsPipelineNode *node) {

        const uint32_t currentSubpass = node->getSubpass();
        rpm.appendReference(node->getAttachmentReference().targetAttachment, node->getAttachmentReference().srcReferenceInfo, currentSubpass);

        const RenderPassAttachment *attachment = node->getAttachmentReference().targetAttachment;

        std::unordered_set<uint32_t> &usedSubpass =
                usedSubpasses.try_emplace(attachment, std::unordered_set<uint32_t>{}).first->second;
        usedSubpass.insert(currentSubpass);

        const std::vector<GraphicsPipelineNode *> &depends = node->getDepends();


        for (const auto depend: depends) {
            const uint32_t dependSubpass = depend->getSubpass();
            rpm.appendReference(depend->getAttachmentReference().targetAttachment, depend->getAttachmentReference().dstReferenceInfo, currentSubpass);

            VkSubpassDependency &dep = generatedDependencies[dependSubpass][currentSubpass];
            dep.srcSubpass = dependSubpass;
            dep.dstSubpass = currentSubpass;
            dep.srcStageMask |= depend->getAttachmentReference().dependency.srcPipelineStageFlags;
            dep.dstStageMask |= depend->getAttachmentReference().dependency.dstPipelineStageFlags;
            dep.srcAccessMask |= depend->getAttachmentReference().dependency.srcAccessFlags;
            dep.dstAccessMask |= depend->getAttachmentReference().dependency.dstAccessFlags;
            dep.dependencyFlags |= depend->getAttachmentReference().dependency.dependencyFlags;
        }
    }
} // namespace merutilm::vkh
