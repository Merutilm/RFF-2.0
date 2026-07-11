//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include <vulkan_helper/engine/manage/PipelineManager.hpp>
#include <vulkan_helper/handle/WindowContextHandler.hpp>
#include "Pipeline.hpp"

namespace merutilm::vkh {
    class ComputeShaderPipeline final : public Pipeline {
    public:
        explicit ComputeShaderPipeline(Core &core, PipelineLayout & pipelineLayout,
                                           PipelineManager &&pipelineManager);

        ~ComputeShaderPipeline() override;

        ComputeShaderPipeline(const ComputeShaderPipeline &) = delete;

        ComputeShaderPipeline &operator=(const ComputeShaderPipeline &) = delete;

        ComputeShaderPipeline(ComputeShaderPipeline &&) = delete;

        ComputeShaderPipeline &operator=(ComputeShaderPipeline &&) = delete;

        void cmdBindAll(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices = {}) const override;


    protected:
        void init() override;
    };


}
