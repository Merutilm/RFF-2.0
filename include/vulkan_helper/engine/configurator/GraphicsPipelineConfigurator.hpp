//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include <vulkan_helper/engine/buffer/IndexBuffer.hpp>
#include <vulkan_helper/engine/buffer/VertexBuffer.hpp>
#include <vulkan_helper/engine/configurator/PipelineConfigurator.hpp>

#include "vulkan_helper/engine/pipeline/GraphicsPipeline.hpp"
#include "vulkan_helper/engine/wrapped/GraphicsPipelineConfiguration.hpp"

namespace merutilm::vkh {
    struct GraphicsPipelineConfigurator : PipelineConfigurator {

        ShaderModule &vertexShader;
        ShaderModule &fragmentShader;

        explicit GraphicsPipelineConfigurator(Engine &engine, WindowContext &wc,
                                              const std::string &vertName, const std::string &fragName) :
            PipelineConfigurator(engine, wc),
            vertexShader(pickFromGlobalRepository<GlobalShaderModuleRepo, ShaderModule &>(vertName)),
            fragmentShader(pickFromGlobalRepository<GlobalShaderModuleRepo, ShaderModule &>(fragName)) {}

        ~GraphicsPipelineConfigurator() override = default;

        GraphicsPipelineConfigurator(const GraphicsPipelineConfigurator &) = delete;

        GraphicsPipelineConfigurator(GraphicsPipelineConfigurator &&) = delete;

        GraphicsPipelineConfigurator &operator=(const GraphicsPipelineConfigurator &) = delete;

        GraphicsPipelineConfigurator &operator=(GraphicsPipelineConfigurator &&) = delete;

    protected:

        virtual void configureVertexBuffer(HostDataObjectManager &som) = 0;

        virtual void configureIndexBuffer(HostDataObjectManager &som) = 0;

        [[nodiscard]] virtual VertexBuffer &getVertexBuffer() = 0;

        [[nodiscard]] virtual IndexBuffer &getIndexBuffer() = 0;

        [[nodiscard]] virtual std::vector<VkGraphicsPipelineCreateInfo> generatePipelineInfo(const PipelineManager &pipelineManager, RenderPass *rp, uint32_t subpass,
                             GraphicsPipelineConfiguration &pipelineConfiguration) = 0;


        void cmdDraw(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t indexVarTarget);

        void createPipeline(PipelineManager &&pipelineManager, RenderPass *rp, uint32_t subpass);
    };

    inline void GraphicsPipelineConfigurator::cmdDraw(const VkCommandBuffer cbh, const uint32_t frameIndex,
                                                      const uint32_t indexVarTarget) {

        const VkBuffer vertexBufferHandle = getVertexBuffer().isMultiframe()
                                                    ? getVertexBuffer().getBufferContextMF(frameIndex).buffer
                                                    : getVertexBuffer().getBufferContext().buffer;
        const VkBuffer indexBufferHandle = getIndexBuffer().isMultiframe()
                                                   ? getIndexBuffer().getBufferContextMF(frameIndex).buffer
                                                   : getIndexBuffer().getBufferContext().buffer;
        constexpr VkDeviceSize vertexBufferOffset = 0;
        {
            vkCmdBindVertexBuffers(cbh, 0, 1, &vertexBufferHandle, &vertexBufferOffset);
            vkCmdBindIndexBuffer(cbh, indexBufferHandle, getIndexBuffer().getHostObject().getOffset(indexVarTarget),
                                 VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(cbh, getIndexBuffer().getHostObject().getElementCount(indexVarTarget), 1, 0, 0, 0);
        }
    }
    inline void GraphicsPipelineConfigurator::createPipeline(PipelineManager &&pipelineManager, RenderPass *rp,
                                                             const uint32_t subpass) {

        GraphicsPipelineConfiguration configuration;
        std::vector<VkGraphicsPipelineCreateInfo> createInfo = generatePipelineInfo(pipelineManager, rp, subpass, configuration);

        pipeline = std::make_unique<GraphicsPipeline>(wc.core, std::move(pipelineManager), std::move(createInfo));
    }
} // namespace merutilm::vkh
