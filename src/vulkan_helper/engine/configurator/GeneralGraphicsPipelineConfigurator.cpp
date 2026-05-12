//
// Created by Merutilm on 2025-07-18.
//

#include <vulkan_helper/engine/pipeline/GraphicsPipeline.hpp>
#include <vulkan_helper/engine/configurator/GeneralGraphicsPipelineConfigurator.hpp>
#include <vulkan_helper/engine/repo/GlobalPipelineLayoutRepo.hpp>
#include <vulkan_helper/engine/repo/Repositories.hpp>

namespace merutilm::vkh {


    void GeneralGraphicsPipelineConfigurator::configure() {
        PipelineLayoutManager pipelineLayoutManager{};

        std::vector<Descriptor *> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            pipelineLayoutManager.appendDescriptorSetLayout(&descriptor->getLayout());
        }

        configurePushConstant(pipelineLayoutManager);
        PipelineLayout & pipelineLayout = engine.getGlobalRepositories().getRepository<GlobalPipelineLayoutRepo>()->pick(
            std::move(pipelineLayoutManager));


        PipelineManager pipelineManager(pipelineLayout);


        pipelineManager.attachDescriptor(std::move(descriptors));
        pipelineManager.attachShader(&vertexShader);
        pipelineManager.attachShader(&fragmentShader);

        HostDataObjectManager vertManager{};
        HostDataObjectManager indexManager{};
        configureVertexBuffer(vertManager);
        configureIndexBuffer(indexManager);

        vertexBuffer.emplace(wc.core, std::move(vertManager), BufferLock::LOCK_UNLOCK, true);
        indexBuffer.emplace(wc.core, std::move(indexManager), BufferLock::LOCK_UNLOCK, true);

        pipeline = std::make_unique<GraphicsPipeline>(wc, pipelineLayout, *vertexBuffer, *indexBuffer, renderContextIndex, primarySubpassIndex,
                                              std::move(pipelineManager));
    }

}
