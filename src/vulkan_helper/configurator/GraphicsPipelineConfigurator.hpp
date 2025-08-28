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


        template<typename Expected> requires std::is_base_of_v<RenderContextConfiguratorAbstract, Expected>
        [[nodiscard]] RenderContextConfiguratorRef getRenderContextConfigurator() const {
            auto r = dynamic_cast<Expected *>(engine.getRenderContext(renderContextIndex).getConfigurator());
            if (r == nullptr) {
                throw exception_invalid_args("Current RenderContext mismatch");
            }
            return *r;
        }


    protected:

        virtual void configureVertexBuffer(HostBufferObjectManagerRef som) = 0;

        virtual void configureIndexBuffer(HostBufferObjectManagerRef som) = 0;

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
