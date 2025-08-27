//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "Pipeline.hpp"
#include "../handle/EngineHandler.hpp"
#include "../manage/PipelineManager.hpp"

namespace merutilm::vkh {
    class ComputeShaderPipelineImpl final : public PipelineAbstract {
    public:
        explicit ComputeShaderPipelineImpl(EngineRef engine, PipelineLayoutRef pipelineLayout,
                                           uint32_t renderContextIndex,
                                           uint32_t primarySubpassIndex,
                                           PipelineManager &&pipelineManager);

        ~ComputeShaderPipelineImpl() override;

        ComputeShaderPipelineImpl(const ComputeShaderPipelineImpl &) = delete;

        ComputeShaderPipelineImpl &operator=(const ComputeShaderPipelineImpl &) = delete;

        ComputeShaderPipelineImpl(ComputeShaderPipelineImpl &&) = delete;

        ComputeShaderPipelineImpl &operator=(ComputeShaderPipelineImpl &&) = delete;

        void bind(VkCommandBuffer cbh, uint32_t frameIndex) const override;

        void init() override;
    };

    using ComputeShaderPipeline = std::unique_ptr<ComputeShaderPipelineImpl>;
    using ComputeShaderPipelinePtr = ComputeShaderPipelineImpl *;
    using ComputeShaderPipelineRef = ComputeShaderPipelineImpl &;
}
