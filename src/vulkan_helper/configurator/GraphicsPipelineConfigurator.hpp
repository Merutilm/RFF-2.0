//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include "PipelineConfigurator.hpp"
#include "../impl/IndexBuffer.hpp"
#include "../impl/VertexBuffer.hpp"

namespace merutilm::vkh {
    struct GraphicsPipelineConfigurator : public PipelineConfiguratorAbstract {
        const uint32_t renderContextIndex;
        const uint32_t primarySubpassIndex;
        ShaderModuleRef vertexShader;
        ShaderModuleRef fragmentShader;

        explicit GraphicsPipelineConfigurator(EngineRef engine, const uint32_t windowContextIndex,
                                              const uint32_t renderContextIndex,
                                              const uint32_t primarySubpassIndex, const std::string &vertName,
                                              const std::string &fragName) : PipelineConfiguratorAbstract(
                                                                                 engine, windowContextIndex),
                                                                             renderContextIndex(renderContextIndex),
                                                                             primarySubpassIndex(primarySubpassIndex),
                                                                             vertexShader(
                                                                                 pickFromGlobalRepository<
                                                                                     GlobalShaderModuleRepo,
                                                                                     ShaderModuleRef>(vertName)),
                                                                             fragmentShader(
                                                                                 pickFromGlobalRepository<
                                                                                     GlobalShaderModuleRepo,
                                                                                     ShaderModuleRef>(fragName)) {
        }

        ~GraphicsPipelineConfigurator() override = default;

        GraphicsPipelineConfigurator(const GraphicsPipelineConfigurator &) = delete;

        GraphicsPipelineConfigurator(GraphicsPipelineConfigurator &&) = delete;

        GraphicsPipelineConfigurator &operator=(const GraphicsPipelineConfigurator &) = delete;

        GraphicsPipelineConfigurator &operator=(GraphicsPipelineConfigurator &&) = delete;

    protected:
        virtual void configureVertexBuffer(HostDataObjectManagerRef som) = 0;

        virtual void configureIndexBuffer(HostDataObjectManagerRef som) = 0;

        [[nodiscard]] virtual VertexBufferRef getVertexBuffer() const = 0;

        [[nodiscard]] virtual IndexBufferRef getIndexBuffer() const = 0;


        void cmdDraw(const VkCommandBuffer cbh, const uint32_t frameIndex, const uint32_t indexVarBinding) const {
            const VkBuffer vertexBufferHandle = getVertexBuffer().isMultiframe()
                                                    ? getVertexBuffer().getBufferContextMF(frameIndex).buffer
                                                    : getVertexBuffer().getBufferContext().buffer;
            const VkBuffer indexBufferHandle = getIndexBuffer().isMultiframe()
                                                   ? getIndexBuffer().getBufferContextMF(frameIndex).buffer
                                                   : getIndexBuffer().getBufferContext().buffer;
            constexpr VkDeviceSize vertexBufferOffset = 0;
            vkCmdBindVertexBuffers(cbh, 0, 1, &vertexBufferHandle, &vertexBufferOffset);
            vkCmdBindIndexBuffer(cbh, indexBufferHandle, getIndexBuffer().getHostObject().getOffset(indexVarBinding),
                                 VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(cbh, getIndexBuffer().getHostObject().getElementCount(indexVarBinding), 1, 0, 0, 0);
        }
    };
}
