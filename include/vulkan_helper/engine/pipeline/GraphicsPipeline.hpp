//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include <vulkan_helper/engine/buffer/VertexBuffer.hpp>
#include <vulkan_helper/engine/buffer/IndexBuffer.hpp>
#include "Pipeline.hpp"

namespace merutilm::vkh {
 
    class GraphicsPipeline final : public Pipeline {

        const uint32_t renderContextIndex;
        const uint32_t primarySubpassIndex;
        VertexBuffer & vertexBuffer;
        IndexBuffer & indexBuffer;

    public:
        explicit GraphicsPipeline(WindowContext & wc, PipelineLayout & pipelineLayout,
                          VertexBuffer & vertexBuffer,
                          IndexBuffer & indexBuffer,
                          uint32_t renderContextIndex,
                          uint32_t primarySubpassIndex,
                          PipelineManager &&pipelineManager);

        ~GraphicsPipeline() override;

        GraphicsPipeline(const GraphicsPipeline &) = delete;

        GraphicsPipeline &operator=(const GraphicsPipeline &) = delete;

        GraphicsPipeline(GraphicsPipeline &&) = delete;

        GraphicsPipeline &operator=(GraphicsPipeline &&) = delete;

        void cmdBindAll(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices = {}) const override;

        [[nodiscard]] VertexBuffer & getVertexBuffer() const { return vertexBuffer; }

        [[nodiscard]] IndexBuffer & getIndexBuffer() const { return indexBuffer; }

    private:
        void init() override;
    };



}
