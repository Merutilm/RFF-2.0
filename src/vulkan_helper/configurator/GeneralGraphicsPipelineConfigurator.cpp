//
// Created by Merutilm on 2025-07-18.
//

#include "GeneralGraphicsPipelineConfigurator.hpp"

#include <utility>

#include "../def/Factory.hpp"
#include "../impl/GraphicsPipeline.hpp"
#include "../repo/SharedDescriptorRepo.hpp"
#include "../repo/Repositories.hpp"

namespace merutilm::vkh {
    GeneralGraphicsPipelineConfigurator::GeneralGraphicsPipelineConfigurator(EngineRef engine,
                                                             const uint32_t renderContextIndex,
                                                             const uint32_t subpassIndex,
                                                             const std::string &vertName,
                                                             const std::string &fragName) : GraphicsPipelineConfigurator(
        engine, renderContextIndex, subpassIndex, vertName, fragName) {
        GeneralGraphicsPipelineConfigurator::init();
    }

    GeneralGraphicsPipelineConfigurator::~GeneralGraphicsPipelineConfigurator() {
        GeneralGraphicsPipelineConfigurator::destroy();
    }

    void GeneralGraphicsPipelineConfigurator::configure() {
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

        auto vertManager = Factory::create<HostBufferObjectManager>();
        auto indexManager = Factory::create<HostBufferObjectManager>();

        configureVertexBuffer(*vertManager);
        configureIndexBuffer(*indexManager);

        vertexBuffer = Factory::create<VertexBuffer>(engine.getCore(), std::move(vertManager), BufferLock::LOCK_UNLOCK);
        indexBuffer = Factory::create<IndexBuffer>(engine.getCore(), std::move(indexManager), BufferLock::LOCK_UNLOCK);

        pipeline = Factory::create<GraphicsPipeline>(engine, pipelineLayout, *vertexBuffer, *indexBuffer, renderContextIndex, primarySubpassIndex,
                                              std::move(pipelineManager));
    }


    void GeneralGraphicsPipelineConfigurator::init() {
        //no operation
    }

    void GeneralGraphicsPipelineConfigurator::destroy() {
        pipeline = nullptr;
        indexBuffer = nullptr;
        vertexBuffer = nullptr;
    }
}
