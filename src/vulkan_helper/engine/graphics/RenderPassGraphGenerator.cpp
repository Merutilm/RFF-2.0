//
// Created by Merutilm on 7/9/26.
//


#include <queue>
#include <ranges>
#include <vulkan_helper/engine/graphics/RenderPassGraphGenerator.hpp>

namespace merutilm::vkh {


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


    void RenderPassGraphGenerator::appendReference(
            std::unordered_map<const RenderPassAttachment *, std::unordered_set<uint32_t>> &usedSubpasses,
            const RenderPassAttachment *attachment, const uint32_t currentSubpass,
            const RenderPassAttachmentReference &referenceInfo) {
        rpm.appendReference(attachment, referenceInfo, currentSubpass);
        std::unordered_set<uint32_t> &usedSubpass =
                usedSubpasses.try_emplace(attachment, std::unordered_set<uint32_t>{}).first->second;
        usedSubpass.insert(currentSubpass);
    }


    void RenderPassGraphGenerator::processNode(
            std::unordered_map<const RenderPassAttachment *, std::unordered_set<uint32_t>> &usedSubpasses,
            std::unordered_map<uint32_t, std::unordered_map<uint32_t, VkSubpassDependency>> &generatedDependencies,
            const GraphicsPipelineNode *node) {

        const uint32_t currentSubpass = node->getSubpass();

        for (auto &ref: node->getAttachmentReference()) {
            const RenderPassAttachment *attachment = ref.targetAttachment;
            appendReference(usedSubpasses, attachment, currentSubpass, ref.srcReferenceInfo);
        }

        const std::vector<GraphicsPipelineNode *> &depends = node->getDepends();


        for (const auto depend: depends) {
            const uint32_t dependSubpass = depend->getSubpass();

            for (auto &ref: depend->getAttachmentReference()) {
                const std::optional<RenderPassAttachmentReference> &dependDstReferenceInfo = ref.dstReferenceInfo;
                const std::optional<SubpassDependency> &dependDstDependency = ref.dependency;

                if (dependDstReferenceInfo.has_value() && dependDstDependency.has_value()) {

                    const RenderPassAttachment *dependsAttachment = ref.targetAttachment;
                    appendReference(usedSubpasses, dependsAttachment, currentSubpass, *dependDstReferenceInfo);

                    VkSubpassDependency &dep = generatedDependencies[dependSubpass][currentSubpass];
                    dep.srcSubpass = dependSubpass;
                    dep.dstSubpass = currentSubpass;
                    dep.srcStageMask |= dependDstDependency->srcPipelineStageFlags;
                    dep.dstStageMask |= dependDstDependency->dstPipelineStageFlags;
                    dep.srcAccessMask |= dependDstDependency->srcAccessFlags;
                    dep.dstAccessMask |= dependDstDependency->dstAccessFlags;
                    dep.dependencyFlags |= dependDstDependency->dependencyFlags;
                } else if (dependDstReferenceInfo.has_value() || dependDstDependency.has_value()) {
                    throw std::invalid_argument("incomplete depend dependency");
                }
            }
        }
    }
} // namespace merutilm::vkh
