//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "../core/vkh_base.hpp"
#include "GraphicsPipelineConfigurator.hpp"
#include "../handle/WindowContextHandler.hpp"
#include "../impl/IndexBuffer.hpp"
#include "../impl/VertexBuffer.hpp"


namespace merutilm::vkh {
    class GeneralGraphicsPipelineConfigurator : public GraphicsPipelineConfigurator {
        VertexBuffer vertexBuffer = {};
        IndexBuffer indexBuffer = nullptr;

    public:
        explicit GeneralGraphicsPipelineConfigurator(EngineRef engine, const uint32_t windowContextIndex,
                                                             const uint32_t renderContextIndex,
                                                             const uint32_t subpassIndex,
                                                             const std::string &vertName,
                                                             const std::string &fragName) : GraphicsPipelineConfigurator(
        engine, windowContextIndex, renderContextIndex, subpassIndex, vertName, fragName) {
        }

        ~GeneralGraphicsPipelineConfigurator() override = default;

        GeneralGraphicsPipelineConfigurator(const GeneralGraphicsPipelineConfigurator &) = delete;

        GeneralGraphicsPipelineConfigurator(GeneralGraphicsPipelineConfigurator &&) = delete;

        GeneralGraphicsPipelineConfigurator &operator=(const GeneralGraphicsPipelineConfigurator &) = delete;

        GeneralGraphicsPipelineConfigurator &operator=(GeneralGraphicsPipelineConfigurator &&) = delete;

        void configure() override;

        [[nodiscard]] VertexBufferRef getVertexBuffer() const override { return *vertexBuffer; }

        [[nodiscard]] IndexBufferRef getIndexBuffer() const override { return *indexBuffer; }

    };
}
