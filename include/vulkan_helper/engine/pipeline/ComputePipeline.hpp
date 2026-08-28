//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include <vulkan_helper/engine/manage/PipelineManager.hpp>
#include <vulkan_helper/handle/WindowContextHandler.hpp>
#include "Pipeline.hpp"

namespace merutilm::vkh {
    class ComputePipeline final : public Pipeline {
        std::vector<VkComputePipelineCreateInfo> createInfos;
    public:
        explicit ComputePipeline(Core &core, PipelineManager &&pipelineManager,
                        std::vector<VkComputePipelineCreateInfo> &&createInfos);

        ~ComputePipeline() override;

        ComputePipeline(const ComputePipeline &) = delete;

        ComputePipeline &operator=(const ComputePipeline &) = delete;

        ComputePipeline(ComputePipeline &&) = delete;

        ComputePipeline &operator=(ComputePipeline &&) = delete;

        void cmdBindAll(VkCommandBuffer cbh, uint32_t specializationIndex, uint32_t frameIndex,
                        DescIndexPicker &&descIndices = {}) const override;


    protected:
        void init() override;
    };


}
