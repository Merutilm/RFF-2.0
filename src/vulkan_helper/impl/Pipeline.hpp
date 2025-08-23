//
// Created by Merutilm on 2025-07-11.
//

#pragma once

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "../manage/PipelineManager.hpp"

#include "../handle/EngineHandler.hpp"

namespace merutilm::vkh {
    class Pipeline final : public EngineHandler {
        VkPipeline pipeline = nullptr;
        const PipelineLayout &pipelineLayout;
        const uint32_t subpassIndex;
        VertexBufferRef vertexBuffer;
        IndexBufferRef indexBuffer;
        const PipelineManager pipelineManager = nullptr;

    public:
        explicit Pipeline(const Engine &engine, const PipelineLayout &pipelineLayout,
                          VertexBufferRef vertexBuffer,
                          IndexBufferRef indexBuffer,
                          uint32_t subpassIndex,
                          PipelineManager &&pipelineManager);

        ~Pipeline() override;

        Pipeline(const Pipeline &) = delete;

        Pipeline &operator=(const Pipeline &) = delete;

        Pipeline(Pipeline &&) = delete;

        Pipeline &operator=(Pipeline &&) = delete;

        void bind( VkCommandBuffer cbh, uint32_t frameIndex) const;

        [[nodiscard]] PipelineManagerRef getPipelineManager() const { return *pipelineManager; }

        [[nodiscard]] const VertexBufferRef getVertexBuffer() const { return vertexBuffer; }

        [[nodiscard]] const IndexBufferRef getIndexBuffer() const { return indexBuffer; }

        [[nodiscard]] VertexBufferRef getVertexBuffer() { return vertexBuffer; }

        [[nodiscard]] IndexBufferRef getIndexBuffer() { return indexBuffer; }

        [[nodiscard]] VkPipeline getPipelineHandle() const { return pipeline; }

    private:
        void init() override;

        void destroy() override;
    };
}
