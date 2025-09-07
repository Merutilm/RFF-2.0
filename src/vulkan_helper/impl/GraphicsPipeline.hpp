//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include "IndexBuffer.hpp"
#include "Pipeline.hpp"
#include "VertexBuffer.hpp"

namespace merutilm::vkh {
 
    class GraphicsPipelineImpl final : public PipelineAbstract {

        const uint32_t renderContextIndex;
        const uint32_t primarySubpassIndex;
        VertexBufferRef vertexBuffer;
        IndexBufferRef indexBuffer;

    public:
        explicit GraphicsPipelineImpl(WindowContextRef wc, PipelineLayoutRef pipelineLayout,
                          VertexBufferRef vertexBuffer,
                          IndexBufferRef indexBuffer,
                          uint32_t renderContextIndex,
                          uint32_t primarySubpassIndex,
                          PipelineManager &&pipelineManager);

        ~GraphicsPipelineImpl() override;

        GraphicsPipelineImpl(const GraphicsPipelineImpl &) = delete;

        GraphicsPipelineImpl &operator=(const GraphicsPipelineImpl &) = delete;

        GraphicsPipelineImpl(GraphicsPipelineImpl &&) = delete;

        GraphicsPipelineImpl &operator=(GraphicsPipelineImpl &&) = delete;

        void cmdBindAll(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) const override;

        [[nodiscard]] VertexBufferRef getVertexBuffer() const { return vertexBuffer; }

        [[nodiscard]] IndexBufferRef getIndexBuffer() const { return indexBuffer; }

    private:
        void init() override;
    };

    using GraphicsPipeline = std::unique_ptr<GraphicsPipelineImpl>;
    using GraphicsPipelinePtr = GraphicsPipelineImpl *;
    using GraphicsPipelineRef = GraphicsPipelineImpl &;

}
