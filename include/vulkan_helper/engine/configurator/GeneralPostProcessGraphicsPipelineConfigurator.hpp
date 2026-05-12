//
// Created by Merutilm on 2025-08-05.
//

#pragma once
#include <vulkan_helper/engine/buffer/IndexBuffer.hpp>
#include <vulkan_helper/engine/buffer/VertexBuffer.hpp>
#include "GeneralGraphicsPipelineConfigurator.hpp"
#include "GraphicsPipelineConfigurator.hpp"

namespace merutilm::vkh {
    class GeneralPostProcessGraphicsPipelineConfigurator : public GraphicsPipelineConfigurator {
        static constexpr auto VERTEX_MODULE_PATH = "vk_post_process.vert";
        VertexBuffer & vertexBufferStaticRef;
        IndexBuffer & indexBufferStaticRef;

    public:

        explicit GeneralPostProcessGraphicsPipelineConfigurator(Engine & engine, const uint32_t windowContextIndex, const uint32_t renderContextIndex, const uint32_t primarySubpassIndex, const std::string &fragName, VertexBuffer &vertexBufferStaticRef, IndexBuffer &indexBufferStaticRef) : GraphicsPipelineConfigurator(
            engine, windowContextIndex, renderContextIndex, primarySubpassIndex, VERTEX_MODULE_PATH, fragName) , vertexBufferStaticRef(vertexBufferStaticRef), indexBufferStaticRef(indexBufferStaticRef){
        }
        ~GeneralPostProcessGraphicsPipelineConfigurator() override = default;

        GeneralPostProcessGraphicsPipelineConfigurator(const GeneralPostProcessGraphicsPipelineConfigurator &) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator &operator=(const GeneralPostProcessGraphicsPipelineConfigurator &) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator(GeneralPostProcessGraphicsPipelineConfigurator &&) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator &operator=(GeneralPostProcessGraphicsPipelineConfigurator &&) = delete;


        void configure() override;

        void cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) override;

    protected:
        [[nodiscard]] VertexBuffer & getVertexBuffer() override { return vertexBufferStaticRef; }

        [[nodiscard]] IndexBuffer & getIndexBuffer() override { return indexBufferStaticRef; }

        void configureVertexBuffer(HostDataObjectManager & som) override;

        void configureIndexBuffer(HostDataObjectManager & som) override;

    };
}
