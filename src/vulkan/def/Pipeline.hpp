//
// Created by Merutilm on 2025-07-11.
//

#pragma once

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "../manage/PipelineManager.hpp"

#include "../handle/EngineHandler.hpp"

namespace merutilm::mvk {
    class Pipeline final : public EngineHandler {
        VkPipeline pipeline = nullptr;
        const PipelineLayout &pipelineLayout;
        const uint32_t subpassIndex;
        VertexBuffer &vertexBuffer;
        IndexBuffer &indexBuffer;
        const std::unique_ptr<PipelineManager> pipelineManager = nullptr;

    public:
        explicit Pipeline(const Engine &engine, const PipelineLayout &pipelineLayout,
                          VertexBuffer &vertexBuffer,
                          IndexBuffer &indexBuffer,
                          uint32_t subpassIndex,
                          std::unique_ptr<PipelineManager> &&pipelineManager);

        ~Pipeline() override;

        Pipeline(const Pipeline &) = delete;

        Pipeline &operator=(const Pipeline &) = delete;

        Pipeline(Pipeline &&) = delete;

        Pipeline &operator=(Pipeline &&) = delete;

        void bind( VkCommandBuffer cbh, uint32_t frameIndex) const;

        [[nodiscard]] const PipelineManager &getPipelineManager() const { return *pipelineManager; }

        [[nodiscard]] PipelineManager &getPipelineManager() { return *pipelineManager; }

        [[nodiscard]] const VertexBuffer &getVertexBuffer() const { return vertexBuffer; }

        [[nodiscard]] const IndexBuffer &getIndexBuffer() const { return indexBuffer; }

        [[nodiscard]] VertexBuffer &getVertexBuffer() { return vertexBuffer; }

        [[nodiscard]] IndexBuffer &getIndexBuffer() { return indexBuffer; }

        [[nodiscard]] VkPipeline getPipelineHandle() const { return pipeline; }

    private:
        void init() override;

        void destroy() override;
    };
}
