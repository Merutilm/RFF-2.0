//
// Created by Merutilm on 2025-08-28.
//

#include "ComputePipelineConfigurator.hpp"

#include "../impl/ComputeShaderPipeline.hpp"

namespace merutilm::vkh {
    ComputePipelineConfigurator::ComputePipelineConfigurator(EngineRef engine,
                                                             const std::string &compName) : PipelineConfigurator(engine),
        computeShader(pickFromRepository<ShaderModuleRepo, ShaderModuleRef>(compName)) {
    }

    void ComputePipelineConfigurator::cmdRender(const VkCommandBuffer cbh, const uint32_t frameIndex) {
        pipeline->cmdBindAll(cbh, frameIndex);
        cmdDispatch(cbh);
    }

    void ComputePipelineConfigurator::configure() {
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
        pipelineManager->attachShader(&computeShader);

        pipeline = Factory::create<ComputeShaderPipeline>(engine, pipelineLayout, std::move(pipelineManager));
    }

    void ComputePipelineConfigurator::init() {
        //noop
    }

    void ComputePipelineConfigurator::destroy() {
        pipeline = nullptr;
    }


}
