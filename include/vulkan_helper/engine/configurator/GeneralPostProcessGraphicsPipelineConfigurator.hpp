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

    public:

        explicit GeneralPostProcessGraphicsPipelineConfigurator(Engine &engine, WindowContext &wc, const std::string &fragName) : GraphicsPipelineConfigurator(
            engine, wc, VERTEX_MODULE_PATH, fragName){
        }
        ~GeneralPostProcessGraphicsPipelineConfigurator() override = default;

        GeneralPostProcessGraphicsPipelineConfigurator(const GeneralPostProcessGraphicsPipelineConfigurator &) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator &operator=(const GeneralPostProcessGraphicsPipelineConfigurator &) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator(GeneralPostProcessGraphicsPipelineConfigurator &&) = delete;

        GeneralPostProcessGraphicsPipelineConfigurator &operator=(GeneralPostProcessGraphicsPipelineConfigurator &&) = delete;

        void cmdRender(VkCommandBuffer cbh, uint32_t frameIndex, DescIndexPicker &&descIndices) override;

        void configure(RenderPass *rp, uint32_t subpass) override;



    protected:
        [[nodiscard]] VertexBuffer & getVertexBuffer() override { return *engine.getSharedResource().vertexBufferIdentity; }

        [[nodiscard]] IndexBuffer & getIndexBuffer() override { return *engine.getSharedResource().indexBufferIdentity; }

        void configureVertexBuffer(HostDataObjectManager & som) override;

        void configureIndexBuffer(HostDataObjectManager & som) override;

    };
}
