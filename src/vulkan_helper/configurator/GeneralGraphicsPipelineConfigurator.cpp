//
// Created by Merutilm on 2025-07-18.
//

#include "GeneralGraphicsPipelineConfigurator.hpp"
#include "../core/vkh_core.hpp"
#include "../impl/GraphicsPipeline.hpp"
#include "../repo/GlobalPipelineLayoutRepo.hpp"
#include "../repo/Repositories.hpp"

namespace merutilm::vkh {


    void GeneralGraphicsPipelineConfigurator::configure() {
        auto pipelineLayoutManager = factory::create<PipelineLayoutManager>();

        std::vector<DescriptorPtr> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            pipelineLayoutManager->appendDescriptorSetLayout(&descriptor->getLayout());
        }

        configurePushConstant(*pipelineLayoutManager);
        PipelineLayoutRef pipelineLayout = engine.getGlobalRepositories().getRepository<GlobalPipelineLayoutRepo>()->pick(
            std::move(pipelineLayoutManager));


        auto pipelineManager = factory::create<PipelineManager>(pipelineLayout);


        pipelineManager->attachDescriptor(std::move(descriptors));
        pipelineManager->attachShader(&vertexShader);
        pipelineManager->attachShader(&fragmentShader);

        auto vertManager = factory::create<HostDataObjectManager>();
        auto indexManager = factory::create<HostDataObjectManager>();

        configureVertexBuffer(*vertManager);
        configureIndexBuffer(*indexManager);

        vertexBuffer = factory::create<VertexBuffer>(wc.core, std::move(vertManager), BufferLock::LOCK_UNLOCK, true);
        indexBuffer = factory::create<IndexBuffer>(wc.core, std::move(indexManager), BufferLock::LOCK_UNLOCK, true);

        pipeline = factory::create<GraphicsPipeline>(wc, pipelineLayout, *vertexBuffer, *indexBuffer, renderContextIndex, primarySubpassIndex,
                                              std::move(pipelineManager));
    }

}
