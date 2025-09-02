//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include "PipelineConfigurator.hpp"
#include "../impl/Pipeline.hpp"
#include "../handle/EngineHandler.hpp"
#include "../impl/IndexBuffer.hpp"
#include "../impl/VertexBuffer.hpp"
#include "../util/DescriptorUpdater.hpp"

namespace merutilm::vkh {
    struct GraphicsPipelineConfigurator : public PipelineConfigurator {

        const uint32_t renderContextIndex;
        const uint32_t primarySubpassIndex;
        ShaderModuleRef vertexShader;
        ShaderModuleRef fragmentShader;

        explicit GraphicsPipelineConfigurator(EngineRef engine, const uint32_t renderContextIndex,
                                              const uint32_t primarySubpassIndex, const std::string &vertName,
                                              const std::string &fragName) : PipelineConfigurator(
                                                                                 engine), renderContextIndex(renderContextIndex), primarySubpassIndex(primarySubpassIndex),
                                                                             vertexShader(pickFromRepository<ShaderModuleRepo, ShaderModuleRef>(vertName)),
                                                                             fragmentShader(pickFromRepository<ShaderModuleRepo, ShaderModuleRef>(fragName)) {
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
            const VkBuffer vertexBufferHandle = getVertexBuffer().getBufferHandle(frameIndex);
            constexpr VkDeviceSize vertexBufferOffset = 0;
            vkCmdBindVertexBuffers(cbh, 0, 1, &vertexBufferHandle, &vertexBufferOffset);
            getIndexBuffer().bind(cbh, frameIndex, indexVarBinding);
            vkCmdDrawIndexed(cbh, getIndexBuffer().getHostObject().getElementCount(indexVarBinding), 1, 0, 0, 0);
        }
    };
}
