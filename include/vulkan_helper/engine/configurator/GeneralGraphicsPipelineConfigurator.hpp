//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>
#include <vulkan_helper/engine/buffer/IndexBuffer.hpp>
#include <vulkan_helper/engine/buffer/VertexBuffer.hpp>
#include "GraphicsPipelineConfigurator.hpp"


namespace merutilm::vkh {
    class GeneralGraphicsPipelineConfigurator : public GraphicsPipelineConfigurator {
        std::optional<VertexBuffer> vertexBuffer;
        std::optional<IndexBuffer> indexBuffer;

    public:
        explicit GeneralGraphicsPipelineConfigurator(Engine &engine, WindowContext &wc,
                                                     const std::string &vertName, const std::string &fragName) :
            GraphicsPipelineConfigurator(engine, wc,  vertName,
                                         fragName) {}

        ~GeneralGraphicsPipelineConfigurator() override = default;

        GeneralGraphicsPipelineConfigurator(const GeneralGraphicsPipelineConfigurator &) = delete;

        GeneralGraphicsPipelineConfigurator(GeneralGraphicsPipelineConfigurator &&) = delete;

        GeneralGraphicsPipelineConfigurator &operator=(const GeneralGraphicsPipelineConfigurator &) = delete;

        GeneralGraphicsPipelineConfigurator &operator=(GeneralGraphicsPipelineConfigurator &&) = delete;

        void configure(RenderPass *rp, uint32_t subpass) override;

    protected:
        [[nodiscard]] VertexBuffer &getVertexBuffer() override { return *vertexBuffer; }

        [[nodiscard]] IndexBuffer &getIndexBuffer() override { return *indexBuffer; }
    };
} // namespace merutilm::vkh
