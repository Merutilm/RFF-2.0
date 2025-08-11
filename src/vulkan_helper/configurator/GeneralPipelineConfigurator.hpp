//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <string>

#include "PipelineConfigurator.hpp"
#include "../context/RenderContext.hpp"
#include "../handle/EngineHandler.hpp"
#include "../def/Pipeline.hpp"


namespace merutilm::vkh {
    class GeneralPipelineConfigurator : public PipelineConfigurator {
        std::unique_ptr<VertexBuffer> vertexBuffer = {};
        std::unique_ptr<IndexBuffer> indexBuffer = nullptr;

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

        [[nodiscard]] VertexBuffer &getVertexBuffer() const override { return *vertexBuffer; }

        [[nodiscard]] IndexBuffer &getIndexBuffer() const override { return *indexBuffer; }

        void configure() override;

    private:
        void init() override;

        void destroy() override;
    };
}
