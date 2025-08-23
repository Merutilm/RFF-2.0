//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <string>

#include "PipelineConfigurator.hpp"
#include "../context/RenderContext.hpp"
#include "../handle/EngineHandler.hpp"
#include "../impl/Pipeline.hpp"


namespace merutilm::vkh {
    class GeneralPipelineConfigurator : public PipelineConfigurator {
        VertexBuffer vertexBuffer = {};
        IndexBuffer indexBuffer = nullptr;

    public:
        explicit GeneralPipelineConfigurator(const Engine &engine,
                                             uint32_t subpassIndex,
                                             const std::string &vertName,
                                             const std::string &fragName);

        ~GeneralPipelineConfigurator() override;

        GeneralPipelineConfigurator(const GeneralPipelineConfigurator &) = delete;

        GeneralPipelineConfigurator(GeneralPipelineConfigurator &&) = delete;

        GeneralPipelineConfigurator &operator=(const GeneralPipelineConfigurator &) = delete;

        GeneralPipelineConfigurator &operator=(GeneralPipelineConfigurator &&) = delete;

        void configure() override;

        [[nodiscard]] VertexBufferRef getVertexBuffer() const override { return *vertexBuffer; }

        [[nodiscard]] IndexBufferRef getIndexBuffer() const override { return *indexBuffer; }

    private:
        void init() override;

        void destroy() override;
    };
}
