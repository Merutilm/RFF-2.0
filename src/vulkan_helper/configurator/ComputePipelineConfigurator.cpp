//
// Created by Merutilm on 2025-08-28.
//

#include "ComputePipelineConfigurator.hpp"
#include "../impl/ComputeShaderPipeline.hpp"
#include "../repo/GlobalPipelineLayoutRepo.hpp"

namespace merutilm::vkh {



    void ComputePipelineConfigurator::cmdRender(const VkCommandBuffer cbh, const uint32_t frameIndex, DescIndexPicker &&descIndices) {
        pipeline->cmdBindAll(cbh, frameIndex, std::move(descIndices));
        cmdDispatch(cbh);
    }

    void ComputePipelineConfigurator::configure() {
        auto pipelineLayoutManager = factory::create<PipelineLayoutManager>();

        std::vector<DescriptorPtr> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            pipelineLayoutManager->appendDescriptorSetLayout(&descriptor->getLayout());
        }

        configurePushConstant(*pipelineLayoutManager);
        PipelineLayoutRef pipelineLayout = pickFromGlobalRepository<GlobalPipelineLayoutRepo, PipelineLayoutRef>(
            std::move(pipelineLayoutManager));


        auto pipelineManager = factory::create<PipelineManager>(pipelineLayout);

        pipelineManager->attachDescriptor(std::move(descriptors));
        pipelineManager->attachShader(&computeShader);

        pipeline = factory::create<ComputeShaderPipeline>(wc, pipelineLayout, std::move(pipelineManager));
    }

}
