//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "Pipeline.hpp"
#include "../handle/WindowContextHandler.hpp"
#include "../manage/PipelineManager.hpp"

namespace merutilm::vkh {
    class ComputeShaderPipelineImpl final : public PipelineAbstract {
    public:
        explicit ComputeShaderPipelineImpl(WindowContextRef wc, PipelineLayoutRef pipelineLayout,
                                           PipelineManager &&pipelineManager);

        ~ComputeShaderPipelineImpl() override;

        ComputeShaderPipelineImpl(const ComputeShaderPipelineImpl &) = delete;

        ComputeShaderPipelineImpl &operator=(const ComputeShaderPipelineImpl &) = delete;

        ComputeShaderPipelineImpl(ComputeShaderPipelineImpl &&) = delete;

        ComputeShaderPipelineImpl &operator=(ComputeShaderPipelineImpl &&) = delete;

        void cmdBindAll(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) const override;

        void init() override;
    };

    using ComputeShaderPipeline = std::unique_ptr<ComputeShaderPipelineImpl>;
    using ComputeShaderPipelinePtr = ComputeShaderPipelineImpl *;
    using ComputeShaderPipelineRef = ComputeShaderPipelineImpl &;
}
