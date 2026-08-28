//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "Pipeline.hpp"

namespace merutilm::vkh {
 
    class GraphicsPipeline final : public Pipeline {
        std::vector<VkGraphicsPipelineCreateInfo> createInfos;
    public:
        explicit GraphicsPipeline(Core &core, PipelineManager &&pipelineManager, std::vector<VkGraphicsPipelineCreateInfo> &&createInfos);

        ~GraphicsPipeline() override;

        GraphicsPipeline(const GraphicsPipeline &) = delete;

        GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;

        GraphicsPipeline(GraphicsPipeline &&) = delete;

        GraphicsPipeline &operator=(GraphicsPipeline &&) = delete;

        void cmdBindAll(VkCommandBuffer cbh, uint32_t specializationIndex, uint32_t frameIndex,
                        DescIndexPicker &&descIndices) const override;

    protected:
        void init() override;
    };



}
