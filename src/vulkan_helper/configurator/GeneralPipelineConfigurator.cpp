//
// Created by Merutilm on 2025-07-18.
//

#include "GeneralPipelineConfigurator.hpp"

#include <utility>

#include "../repo/SharedDescriptorRepo.hpp"
#include "../repo/Repositories.hpp"

namespace merutilm::mvk {
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

        auto vertManager = std::make_unique<ShaderObjectManager>();
        auto indexManager = std::make_unique<ShaderObjectManager>();

        configureVertexBuffer(*vertManager);
        configureIndexBuffer(*indexManager);

        vertexBuffer = std::make_unique<VertexBuffer>(engine.getCore(), std::move(vertManager));
        indexBuffer = std::make_unique<IndexBuffer>(engine.getCore(), std::move(indexManager));

        pipeline = std::make_unique<Pipeline>(engine, pipelineLayout, vertexBuffer.get(), indexBuffer.get(), subpassIndex,
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
