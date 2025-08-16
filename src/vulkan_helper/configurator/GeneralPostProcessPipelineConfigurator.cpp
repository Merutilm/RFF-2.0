//
// Created by Merutilm on 2025-08-05.
//

#include "GeneralPostProcessPipelineConfigurator.hpp"

#include "../struct/Vertex.hpp"

namespace merutilm::vkh {

    GeneralPostProcessPipelineConfigurator::GeneralPostProcessPipelineConfigurator(const Engine &engine, uint32_t subpassIndex, const std::string &fragName)  : PipelineConfigurator(
            engine, subpassIndex, VERTEX_MODULE_PATH, fragName) {
        GeneralPostProcessPipelineConfigurator::init();
    }
    GeneralPostProcessPipelineConfigurator::~GeneralPostProcessPipelineConfigurator() {
        GeneralPostProcessPipelineConfigurator::destroy();
    }


    void GeneralPostProcessPipelineConfigurator::render(const VkCommandBuffer cbh, const uint32_t frameIndex, uint32_t width, uint32_t height) {
        pushAll(cbh);
        draw(cbh, frameIndex, 0);
    }


    void GeneralPostProcessPipelineConfigurator::configureVertexBuffer(HostBufferObjectManager &som) {
        som.addArray(0, std::vector{
                    Vertex::generate({1, 1, 0}, {1, 1, 1}, {1, 1}),
                    Vertex::generate({1, -1, 0}, {1, 1, 1}, {1, 0}),
                    Vertex::generate({-1, -1, 0}, {1, 1, 1}, {0, 0}),
                    Vertex::generate({-1, 1, 0}, {1, 1, 1}, {0, 1}),
                });
    }

    void GeneralPostProcessPipelineConfigurator::configureIndexBuffer(HostBufferObjectManager &som) {
        som.addArray(0, std::vector<uint32_t>{0, 1, 2, 2, 3, 0});
    }

    void GeneralPostProcessPipelineConfigurator::configure() {
        auto pipelineLayoutManager = std::make_unique<PipelineLayoutManager>();
        auto &layoutRepo = *engine.getRepositories().getRepository<DescriptorSetLayoutRepo>();

        std::vector<const Descriptor *> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            const DescriptorSetLayout &layout = layoutRepo.pick(descriptor->getDescriptorManager().getLayoutBuilder());
            pipelineLayoutManager->appendDescriptorSetLayout(&layout);
        }

        configurePushConstant(*pipelineLayoutManager);
        const PipelineLayout &pipelineLayout = engine.getRepositories().getRepository<PipelineLayoutRepo>()->pick(
            std::move(pipelineLayoutManager));


        auto pipelineManager = std::make_unique<PipelineManager>(pipelineLayout);

        pipelineManager->attachDescriptor(std::move(descriptors));
        pipelineManager->attachShader(&vertexShader);
        pipelineManager->attachShader(&fragmentShader);


        if (!initializedVertexIndex) {
            auto vertManager = std::make_unique<HostBufferObjectManager>();
            auto indexManager = std::make_unique<HostBufferObjectManager>();

            configureVertexBuffer(*vertManager);
            configureIndexBuffer(*indexManager);

            vertexBufferPP = std::make_unique<VertexBuffer>(engine.getCore(), std::move(vertManager), BufferLock::LOCK_ONLY);
            indexBufferPP = std::make_unique<IndexBuffer>(engine.getCore(), std::move(indexManager), BufferLock::LOCK_ONLY);
            for (int i = 0; i < engine.getCore().getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                vertexBufferPP->update(i);
                indexBufferPP->update(i);
            }
            vertexBufferPP->lock(engine.getCommandPool());
            indexBufferPP->lock(engine.getCommandPool());
            initializedVertexIndex = true;
        }

        if (initializedVertexIndex) {
            pipeline = std::make_unique<Pipeline>(engine, pipelineLayout, getVertexBuffer(), getIndexBuffer(),
                                                  subpassIndex,
                                                  std::move(pipelineManager));
        }
    }

    void GeneralPostProcessPipelineConfigurator::init() {
        //no operation
    }

    void GeneralPostProcessPipelineConfigurator::destroy() {
        pipeline = nullptr;
    }
}
