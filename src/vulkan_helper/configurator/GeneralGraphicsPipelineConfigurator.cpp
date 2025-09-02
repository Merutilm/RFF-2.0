//
// Created by Merutilm on 2025-07-18.
//

#include "GeneralGraphicsPipelineConfigurator.hpp"
#include "../core/vkh_core.hpp"
#include "../impl/GraphicsPipeline.hpp"
#include "../repo/Repositories.hpp"

namespace merutilm::vkh {
    GeneralGraphicsPipelineConfigurator::GeneralGraphicsPipelineConfigurator(EngineRef engine,
                                                             const uint32_t renderContextIndex,
                                                             const uint32_t subpassIndex,
                                                             const std::string &vertName,
                                                             const std::string &fragName) : GraphicsPipelineConfigurator(
        engine, renderContextIndex, subpassIndex, vertName, fragName) {
    }


    void GeneralGraphicsPipelineConfigurator::configure() {
        auto pipelineLayoutManager = factory::create<PipelineLayoutManager>();

        std::vector<DescriptorPtr> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            pipelineLayoutManager->appendDescriptorSetLayout(&descriptor->getLayout());
        }

        configurePushConstant(*pipelineLayoutManager);
        PipelineLayoutRef pipelineLayout = engine.getRepositories().getRepository<PipelineLayoutRepo>()->pick(
            std::move(pipelineLayoutManager));


        auto pipelineManager = factory::create<PipelineManager>(pipelineLayout);


        pipelineManager->attachDescriptor(std::move(descriptors));
        pipelineManager->attachShader(&vertexShader);
        pipelineManager->attachShader(&fragmentShader);

        auto vertManager = factory::create<HostDataObjectManager>();
        auto indexManager = factory::create<HostDataObjectManager>();

        configureVertexBuffer(*vertManager);
        configureIndexBuffer(*indexManager);

        vertexBuffer = factory::create<VertexBuffer>(engine.getCore(), std::move(vertManager), BufferLock::LOCK_UNLOCK);
        indexBuffer = factory::create<IndexBuffer>(engine.getCore(), std::move(indexManager), BufferLock::LOCK_UNLOCK);

        pipeline = factory::create<GraphicsPipeline>(engine, pipelineLayout, *vertexBuffer, *indexBuffer, renderContextIndex, primarySubpassIndex,
                                              std::move(pipelineManager));
    }

}
