//
// Created by Merutilm on 7/7/26.
//

#pragma once


#include "vulkan_helper/engine/pipeline/GraphicsPipelineNode.hpp"
#include "RenderPassGraphGeneratorBase.hpp"
#include <vulkan_helper/engine/Engine.hpp>

namespace merutilm::vkh {
    struct RenderPassGraphGenerator : RenderPassGraphGeneratorBase {

        Engine &engine;
        WindowContext &wc;
        std::function<MultiframeImageContext()> swapchainImageContextGetter;

        template<typename F>
            requires std::is_invocable_r_v<MultiframeImageContext, F>
        explicit RenderPassGraphGenerator(Engine &engine, WindowContext &wc, F &&swapchainImageContextGetter);

        ~RenderPassGraphGenerator() override = default;
        RenderPassGraphGenerator(const RenderPassGraphGenerator &) = delete;
        RenderPassGraphGenerator &operator=(const RenderPassGraphGenerator &) = delete;
        RenderPassGraphGenerator(RenderPassGraphGenerator &&) = delete;
        RenderPassGraphGenerator &operator=(RenderPassGraphGenerator &&) = delete;


        [[nodiscard]] RenderPassAttachment &appendAttachment(const VkAttachmentDescription &attachmentDescription,
                                               const MultiframeImageContext &imageContext);


        void createPipelines(RenderPass *rp) const override;

    protected:
        template<typename Tp, typename Func>
            requires std::is_base_of_v<GraphicsPipelineConfigurator, Tp> && std::is_invocable_r_v<DescIndexPicker, Func>
        GraphicsPipelineNode *registerPipeline(Tp ** result, std::vector<GraphicsPipelineNode *> &&depends,
                                           RenderPassAttachmentReferenceDetail &&reference, Func &&descIndexPickerFunc);

        void generate() override;

    private:
        void processNode(
                std::unordered_map<const RenderPassAttachment *, std::unordered_set<uint32_t>> &usedSubpasses,
                std::unordered_map<uint32_t, std::unordered_map<uint32_t, VkSubpassDependency>> &generatedDependencies,
                const GraphicsPipelineNode *node);
    };


    template<typename F>
        requires std::is_invocable_r_v<std::vector<ImageContext>, F>
    RenderPassGraphGenerator::RenderPassGraphGenerator(Engine &engine, WindowContext &wc,
                                                       F &&swapchainImageContextGetter) :
        engine(engine), wc(wc), swapchainImageContextGetter(std::forward<F>(swapchainImageContextGetter)) {}


    template<typename Tp, typename Func>
        requires std::is_base_of_v<GraphicsPipelineConfigurator, Tp> && std::is_invocable_r_v<DescIndexPicker, Func>
    GraphicsPipelineNode *RenderPassGraphGenerator::registerPipeline(Tp ** result, std::vector<GraphicsPipelineNode *> &&depends,
                                                                 RenderPassAttachmentReferenceDetail &&reference, Func &&descIndexPickerFunc) {
        uint32_t subpassIndex = 0;

        if (depends.empty()) {
            subpassIndex = 0;
        } else {
            for (const auto depend: depends) {
                subpassIndex = std::max(subpassIndex, depend->getSubpass());
            }
            ++subpassIndex;
        }

        if (subpassIndex >= createdSubpassesCount) { // it is not a while loop.
            rpm.appendSubpass();
            ++createdSubpassesCount;
        }
        auto ptr = dynamic_cast<Tp *>(
                pipelineConfiguratorsCache
                        .emplace_back(std::make_unique<Tp>(engine, wc))
                        .get());

        if (result) *result = ptr;

        return pipelines
                .emplace_back(std::make_unique<GraphicsPipelineNode>(std::move(depends), ptr, std::move(reference),
                                                                     subpassIndex, std::forward<Func>(descIndexPickerFunc)))
                .get();
    }

} // namespace merutilm::vkh
