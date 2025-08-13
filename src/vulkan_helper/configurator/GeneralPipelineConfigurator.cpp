//
// Created by Merutilm on 2025-07-18.
//

#include "GeneralPipelineConfigurator.hpp"

#include <utility>

#include "../repo/SharedDescriptorRepo.hpp"
#include "../repo/Repositories.hpp"

namespace merutilm::vkh {
    GeneralPipelineConfigurator::GeneralPipelineConfigurator(const Engine &engine,
                                                             const uint32_t subpassIndex,
                                                             const std::string &vertName,
                                                             const std::string &fragName) : PipelineConfigurator(engine, subpassIndex, vertName, fragName){
        GeneralPipelineConfigurator::init();
    }

    GeneralPipelineConfigurator::~GeneralPipelineConfigurator() {
        GeneralPipelineConfigurator::destroy();
    }


    void GeneralPipelineConfigurator::configure() {
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

        auto vertManager = std::make_unique<BufferObjectManager>();
        auto indexManager = std::make_unique<BufferObjectManager>();

        configureVertexBuffer(*vertManager);
        configureIndexBuffer(*indexManager);

        vertexBuffer = std::make_unique<VertexBuffer>(engine.getCore(), std::move(vertManager), BufferLock::LOCK_UNLOCK);
        indexBuffer = std::make_unique<IndexBuffer>(engine.getCore(), std::move(indexManager), BufferLock::LOCK_UNLOCK);

        pipeline = std::make_unique<Pipeline>(engine, pipelineLayout, *vertexBuffer, *indexBuffer, subpassIndex,
                                              std::move(pipelineManager));
    }


    void GeneralPipelineConfigurator::init() {
        //no operation
    }

    void GeneralPipelineConfigurator::destroy() {
        pipeline = nullptr;
        indexBuffer = nullptr;
        vertexBuffer = nullptr;
    }
}
