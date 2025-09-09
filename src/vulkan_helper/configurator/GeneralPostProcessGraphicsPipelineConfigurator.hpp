//
// Created by Merutilm on 2025-08-05.
//

#pragma once
#include "GeneralGraphicsPipelineConfigurator.hpp"
#include "GraphicsPipelineConfigurator.hpp"
#include "../impl/IndexBuffer.hpp"
#include "../impl/VertexBuffer.hpp"

namespace merutilm::vkh {
    class GeneralPostProcessGraphicsPipelineConfigurator : public GraphicsPipelineConfigurator {
        inline static bool initializedVertexIndex = false;
        inline static VertexBuffer vertexBufferPP = nullptr;
        inline static IndexBuffer indexBufferPP = nullptr;
        static constexpr auto VERTEX_MODULE_PATH = "vk_post_process.vert";

    public:

        explicit GeneralPostProcessGraphicsPipelineConfigurator(EngineRef engine, const uint32_t windowContextIndex, const uint32_t renderContextIndex, const uint32_t primarySubpassIndex, const std::string &fragName) : GraphicsPipelineConfigurator(
            engine, windowContextIndex, renderContextIndex, primarySubpassIndex, VERTEX_MODULE_PATH, fragName) {
        }
        ~GeneralPostProcessGraphicsPipelineConfigurator() override = default;

        GeneralPostProcessGraphicsPipelineConfigurator(const GeneralPostProcessGraphicsPipelineConfigurator &) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator &operator=(const GeneralPostProcessGraphicsPipelineConfigurator &) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator(GeneralPostProcessGraphicsPipelineConfigurator &&) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator &operator=(GeneralPostProcessGraphicsPipelineConfigurator &&) = delete;


        void configure() override;

        void cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) override;

        static void cleanup() {
            vertexBufferPP = nullptr;
            indexBufferPP = nullptr;
            initializedVertexIndex = false;
        }

    protected:
        [[nodiscard]] VertexBufferRef getVertexBuffer() const override { return *vertexBufferPP; }

        [[nodiscard]] IndexBufferRef getIndexBuffer() const override { return *indexBufferPP; }

        void configureVertexBuffer(HostDataObjectManagerRef som) override;

        void configureIndexBuffer(HostDataObjectManagerRef som) override;

    };
}
