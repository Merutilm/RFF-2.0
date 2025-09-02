//
// Created by Merutilm on 2025-08-05.
//

#include "GeneralPostProcessGraphicsPipelineConfigurator.hpp"

#include "../core/vkh_core.hpp"
#include "../impl/GraphicsPipeline.hpp"
#include "../struct/Vertex.hpp"

namespace merutilm::vkh {

    GeneralPostProcessGraphicsPipelineConfigurator::GeneralPostProcessGraphicsPipelineConfigurator(EngineRef engine, const uint32_t renderContextIndex, const uint32_t primarySubpassIndex, const std::string &fragName) : GraphicsPipelineConfigurator(
            engine, renderContextIndex, primarySubpassIndex, VERTEX_MODULE_PATH, fragName) {
        GeneralPostProcessGraphicsPipelineConfigurator::init();
    }
    GeneralPostProcessGraphicsPipelineConfigurator::~GeneralPostProcessGraphicsPipelineConfigurator() {
        GeneralPostProcessGraphicsPipelineConfigurator::destroy();
    }


    void GeneralPostProcessGraphicsPipelineConfigurator::cmdRender(const VkCommandBuffer cbh, const uint32_t frameIndex, DescIndexPicker &&descIndices) {
        pipeline->cmdBindAll(cbh, frameIndex, std::move(descIndices));
        cmdPushAll(cbh);
        cmdDraw(cbh, frameIndex, 0);
    }


    void GeneralPostProcessGraphicsPipelineConfigurator::configureVertexBuffer(HostDataObjectManagerRef som) {
        som.addArray(0, std::vector{
                    Vertex::generate({1, 1, 0}, {1, 1, 1}, {1, 1}),
                    Vertex::generate({1, -1, 0}, {1, 1, 1}, {1, 0}),
                    Vertex::generate({-1, -1, 0}, {1, 1, 1}, {0, 0}),
            Vertex::generate({-1, 1, 0}, {1, 1, 1}, {0, 1}),
                });
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::configureIndexBuffer(HostDataObjectManagerRef som) {
        som.addArray(0, std::vector<uint32_t>{0, 1, 2, 2, 3, 0});
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::configure() {
        auto pipelineLayoutManager = factory::create<PipelineLayoutManager>();

        std::vector<DescriptorPtr> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor : descriptors) {
            pipelineLayoutManager->appendDescriptorSetLayout(&descriptor->getLayout());
        }

        configurePushConstant(*pipelineLayoutManager);
        PipelineLayoutRef pipelineLayout = engine.getRepositories().getRepository<PipelineLayoutRepo>()->pick(
            std::move(pipelineLayoutManager));


        auto pipelineManager = factory::create<PipelineManager>(pipelineLayout);

        pipelineManager->attachDescriptor(std::move(descriptors));
        pipelineManager->attachShader(&vertexShader);
        pipelineManager->attachShader(&fragmentShader);


        if (!initializedVertexIndex) {
            auto vertManager = factory::create<HostDataObjectManager>();;
            auto indexManager = factory::create<HostDataObjectManager>();;

            configureVertexBuffer(*vertManager);
            configureIndexBuffer(*indexManager);

            vertexBufferPP = factory::create<VertexBuffer>(engine.getCore(), std::move(vertManager), BufferLock::LOCK_ONLY);
            indexBufferPP = factory::create<IndexBuffer>(engine.getCore(), std::move(indexManager), BufferLock::LOCK_ONLY);
            for (int i = 0; i < engine.getCore().getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                vertexBufferPP->update(i);
                indexBufferPP->update(i);
            }
            vertexBufferPP->lock(engine.getCommandPool());
            indexBufferPP->lock(engine.getCommandPool());
            initializedVertexIndex = true;
        }

        if (initializedVertexIndex) {
            pipeline = factory::create<GraphicsPipeline>(engine, pipelineLayout, getVertexBuffer(), getIndexBuffer(), renderContextIndex,
                                                  primarySubpassIndex,
                                                  std::move(pipelineManager));
        }
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::init() {
        //no operation
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::destroy() {
        pipeline = nullptr;
    }
}
