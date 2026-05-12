//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/engine/buffer/IndexBuffer.hpp>
#include <vulkan_helper/engine/buffer/VertexBuffer.hpp>
#include <vulkan_helper/handle/WindowContextHandler.hpp>
#include "GraphicsPipelineConfigurator.hpp"


namespace merutilm::vkh {
    class GeneralGraphicsPipelineConfigurator : public GraphicsPipelineConfigurator {
        std::optional<VertexBuffer> vertexBuffer;
        std::optional<IndexBuffer> indexBuffer;

    public:
        explicit GeneralGraphicsPipelineConfigurator(Engine &engine, const uint32_t windowContextIndex,
                                                     const uint32_t renderContextIndex, const uint32_t subpassIndex,
                                                     const std::string &vertName, const std::string &fragName) :
            GraphicsPipelineConfigurator(engine, windowContextIndex, renderContextIndex, subpassIndex, vertName,
                                         fragName) {}

        ~GeneralGraphicsPipelineConfigurator() override = default;

        GeneralGraphicsPipelineConfigurator(const GeneralGraphicsPipelineConfigurator &) = delete;

        GeneralGraphicsPipelineConfigurator(GeneralGraphicsPipelineConfigurator &&) = delete;

        GeneralGraphicsPipelineConfigurator &operator=(const GeneralGraphicsPipelineConfigurator &) = delete;

        GeneralGraphicsPipelineConfigurator &operator=(GeneralGraphicsPipelineConfigurator &&) = delete;

        void configure() override;

        [[nodiscard]] VertexBuffer &getVertexBuffer() override { return *vertexBuffer; }

        [[nodiscard]] IndexBuffer &getIndexBuffer() override { return *indexBuffer; }
    };
} // namespace merutilm::vkh
