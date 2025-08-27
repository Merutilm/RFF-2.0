//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <string>

#include "GraphicsPipelineConfigurator.hpp"
#include "../context/RenderContext.hpp"
#include "../handle/EngineHandler.hpp"
#include "../impl/IndexBuffer.hpp"
#include "../impl/VertexBuffer.hpp"


namespace merutilm::vkh {
    class GeneralGraphicsPipelineConfigurator : public GraphicsPipelineConfigurator {
        VertexBuffer vertexBuffer = {};
        IndexBuffer indexBuffer = nullptr;

    public:
        explicit GeneralGraphicsPipelineConfigurator(EngineRef engine,
                                             uint32_t renderContextIndex,
                                             uint32_t subpassIndex,
                                             const std::string &vertName,
                                             const std::string &fragName);

        ~GeneralGraphicsPipelineConfigurator() override;

        GeneralGraphicsPipelineConfigurator(const GeneralGraphicsPipelineConfigurator &) = delete;

        GeneralGraphicsPipelineConfigurator(GeneralGraphicsPipelineConfigurator &&) = delete;

        GeneralGraphicsPipelineConfigurator &operator=(const GeneralGraphicsPipelineConfigurator &) = delete;

        GeneralGraphicsPipelineConfigurator &operator=(GeneralGraphicsPipelineConfigurator &&) = delete;

        void configure() override;

        [[nodiscard]] VertexBufferRef getVertexBuffer() const override { return *vertexBuffer; }

        [[nodiscard]] IndexBufferRef getIndexBuffer() const override { return *indexBuffer; }

    private:
        void init() override;

        void destroy() override;
    };
}
