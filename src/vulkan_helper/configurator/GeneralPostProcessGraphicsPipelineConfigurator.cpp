//
// Created by Merutilm on 2025-08-05.
//

#include "GeneralPostProcessGraphicsPipelineConfigurator.hpp"

#include "../def/Factory.hpp"
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


    void GeneralPostProcessGraphicsPipelineConfigurator::render(const VkCommandBuffer cbh, const uint32_t frameIndex) {
        pushAll(cbh);
        draw(cbh, frameIndex, 0);
    }


    void GeneralPostProcessGraphicsPipelineConfigurator::configureVertexBuffer(HostBufferObjectManagerRef som) {
        som.addArray(0, std::vector{
                    Vertex::generate({1, 1, 0}, {1, 1, 1}, {1, 1}),
                    Vertex::generate({1, -1, 0}, {1, 1, 1}, {1, 0}),
                    Vertex::generate({-1, -1, 0}, {1, 1, 1}, {0, 0}),
                    Vertex::generate({-1, 1, 0}, {1, 1, 1}, {0, 1}),
                });
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::configureIndexBuffer(HostBufferObjectManagerRef som) {
        som.addArray(0, std::vector<uint32_t>{0, 1, 2, 2, 3, 0});
    }

    void GeneralPostProcessGraphicsPipelineConfigurator::configure() {
        auto pipelineLayoutManager = Factory::create<PipelineLayoutManager>();
        auto &layoutRepo = *engine.getRepositories().getRepository<DescriptorSetLayoutRepo>();

        std::vector<DescriptorPtr> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            DescriptorSetLayoutRef layout = layoutRepo.pick(descriptor->getDescriptorManager().getLayoutBuilder());
            pipelineLayoutManager->appendDescriptorSetLayout(&layout);
        }

        configurePushConstant(*pipelineLayoutManager);
        PipelineLayoutRef pipelineLayout = engine.getRepositories().getRepository<PipelineLayoutRepo>()->pick(
            std::move(pipelineLayoutManager));


        auto pipelineManager = Factory::create<PipelineManager>(pipelineLayout);

        pipelineManager->attachDescriptor(std::move(descriptors));
        pipelineManager->attachShader(&vertexShader);
        pipelineManager->attachShader(&fragmentShader);


        if (!initializedVertexIndex) {
            auto vertManager = Factory::create<HostBufferObjectManager>();;
            auto indexManager = Factory::create<HostBufferObjectManager>();;

            configureVertexBuffer(*vertManager);
            configureIndexBuffer(*indexManager);

            vertexBufferPP = Factory::create<VertexBuffer>(engine.getCore(), std::move(vertManager), BufferLock::LOCK_ONLY);
            indexBufferPP = Factory::create<IndexBuffer>(engine.getCore(), std::move(indexManager), BufferLock::LOCK_ONLY);
            for (int i = 0; i < engine.getCore().getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                vertexBufferPP->update(i);
                indexBufferPP->update(i);
            }
            vertexBufferPP->lock(engine.getCommandPool());
            indexBufferPP->lock(engine.getCommandPool());
            initializedVertexIndex = true;
        }

        if (initializedVertexIndex) {
            pipeline = Factory::create<GraphicsPipeline>(engine, pipelineLayout, getVertexBuffer(), getIndexBuffer(), renderContextIndex,
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
