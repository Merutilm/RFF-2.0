//
// Created by Merutilm on 2025-08-27.
//

#pragma once
#include <vulkan_helper/engine/buffer/VertexBuffer.hpp>
#include <vulkan_helper/engine/buffer/IndexBuffer.hpp>
#include <vulkan_helper/engine/graphics/RenderPass.hpp>
#include "Pipeline.hpp"

namespace merutilm::vkh {
 
    class GraphicsPipeline final : public Pipeline {

        RenderPass &rp;
        const uint32_t primarySubpassIndex;
        VertexBuffer & vertexBuffer;
        IndexBuffer & indexBuffer;

    public:
        explicit GraphicsPipeline(Core &core, PipelineLayout & pipelineLayout,
                          VertexBuffer & vertexBuffer,
                          IndexBuffer & indexBuffer,
                          RenderPass &rp,
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

    protected:
        void init() override;
    };



}
