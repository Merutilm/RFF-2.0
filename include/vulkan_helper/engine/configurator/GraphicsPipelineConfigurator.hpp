//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include <vulkan_helper/engine/buffer/IndexBuffer.hpp>
#include <vulkan_helper/engine/buffer/VertexBuffer.hpp>
#include <vulkan_helper/engine/configurator/PipelineConfigurator.hpp>

namespace merutilm::vkh {
    struct GraphicsPipelineConfigurator : PipelineConfigurator {
        const uint32_t renderContextIndex;
        const uint32_t primarySubpassIndex;
        ShaderModule &vertexShader;
        ShaderModule &fragmentShader;

        explicit GraphicsPipelineConfigurator(Engine &engine, const uint32_t windowContextIndex,
                                              const uint32_t renderContextIndex, const uint32_t primarySubpassIndex,
                                              const std::string &vertName, const std::string &fragName) :
            PipelineConfigurator(engine, windowContextIndex), renderContextIndex(renderContextIndex),
            primarySubpassIndex(primarySubpassIndex),
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


        void cmdDraw(const VkCommandBuffer cbh, const uint32_t frameIndex, const uint32_t indexVarBinding) {
            const VkBuffer vertexBufferHandle = getVertexBuffer().isMultiframe()
                                                        ? getVertexBuffer().getBufferContextMF(frameIndex).buffer
                                                        : getVertexBuffer().getBufferContext().buffer;
            const VkBuffer indexBufferHandle = getIndexBuffer().isMultiframe()
                                                       ? getIndexBuffer().getBufferContextMF(frameIndex).buffer
                                                       : getIndexBuffer().getBufferContext().buffer;
            constexpr VkDeviceSize vertexBufferOffset = 0;
            vkCmdBindVertexBuffers(cbh, 0, 1, &vertexBufferHandle, &vertexBufferOffset);
            vkCmdBindIndexBuffer(cbh, indexBufferHandle, getIndexBuffer().getHostObject().getOffset(indexVarBinding), VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(cbh, getIndexBuffer().getHostObject().getElementCount(indexVarBinding), 1, 0, 0, 0);
        }
    };
} // namespace merutilm::vkh
