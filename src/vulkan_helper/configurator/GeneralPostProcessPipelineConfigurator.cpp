//
// Created by Merutilm on 2025-08-05.
//

#include "GeneralPostProcessPipelineConfigurator.hpp"

#include "../struct/Vertex.hpp"

namespace merutilm::vkh {



    void GeneralPostProcessPipelineConfigurator::configure() {
        auto pipelineLayoutManager = std::make_unique<PipelineLayoutManager>();
        auto &layoutRepo = *engine.getRepositories().getRepository<DescriptorSetLayoutRepo>();
        auto &descRepo = *engine.getRepositories().getRepository<SharedDescriptorRepo>();

        std::vector<const Descriptor *> descriptors = {};
        configureDescriptors(descriptors, layoutRepo, descRepo);

        for (const auto descriptor: descriptors) {
            const DescriptorSetLayout &layout = layoutRepo.pick(descriptor->getDescriptorManager().getLayoutBuilder());
            pipelineLayoutManager->appendDescriptorSetLayout(&layout);
        }

        configurePushConstant(layoutRepo, *pipelineLayoutManager);
        const PipelineLayout &pipelineLayout = engine.getRepositories().getRepository<PipelineLayoutRepo>()->pick(
            std::move(pipelineLayoutManager));


        auto pipelineManager = std::make_unique<PipelineManager>(pipelineLayout);

        pipelineManager->attachDescriptor(std::move(descriptors));
        pipelineManager->attachShader(&vertexShader);
        pipelineManager->attachShader(&fragmentShader);


        if (!initializedVertexIndex) {
            auto vertManager = std::make_unique<ShaderObjectManager>();
            auto indexManager = std::make_unique<ShaderObjectManager>();

            configureVertexBuffer(*vertManager);
            configureIndexBuffer(*indexManager);

            vertexBufferPP = std::make_unique<VertexBuffer>(engine.getCore(), std::move(vertManager));
            indexBufferPP = std::make_unique<IndexBuffer>(engine.getCore(), std::move(indexManager));
            for (int i = 0; i < engine.getCore().getPhysicalDevice().getMaxFramesInFlight(); ++i) {
                vertexBufferPP->update(i);
                indexBufferPP->update(i);
            }
            vertexBufferPP->finalize(engine.getCommandPool());
            indexBufferPP->finalize(engine.getCommandPool());
            initializedVertexIndex = true;
        }

        if (initializedVertexIndex) {
            pipeline = std::make_unique<Pipeline>(engine, pipelineLayout, vertexBufferPP.get(), indexBufferPP.get(),
                                                  subpassIndex,
                                                  std::move(pipelineManager));
        }
    }


    void GeneralPostProcessPipelineConfigurator::configureVertexBuffer(ShaderObjectManager &som) {
        som.addArray(0, std::vector{
                    Vertex::generate({1, 1, 0}, {1, 1, 1}, {1, 1}),
                    Vertex::generate({1, -1, 0}, {1, 1, 1}, {1, 0}),
                    Vertex::generate({-1, -1, 0}, {1, 1, 1}, {0, 0}),
                    Vertex::generate({-1, 1, 0}, {1, 1, 1}, {0, 1}),
                });
    }

    void GeneralPostProcessPipelineConfigurator::configureIndexBuffer(ShaderObjectManager &som) {
        som.addArray(0, std::vector<uint32_t>{0, 1, 2, 2, 3, 0});
    }

    void GeneralPostProcessPipelineConfigurator::init() {
        //no operation
    }

    void GeneralPostProcessPipelineConfigurator::destroy() {
        //no operation
    }
}
