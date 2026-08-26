//
// Created by Merutilm on 2025-08-28.
//

#include <vulkan_helper/engine/pipeline/ComputePipeline.hpp>
#include <vulkan_helper/engine/configurator/ComputePipelineConfigurator.hpp>
#include <vulkan_helper/engine/repo/GlobalPipelineLayoutRepo.hpp>

namespace merutilm::vkh {



    void ComputePipelineConfigurator::cmdRender(const VkCommandBuffer cbh,
                                                const uint32_t frameIndex, DescIndexPicker &&descIndices) {
        pipeline->cmdBindAll(cbh, specializationIndex, frameIndex, std::move(descIndices));
        cmdDispatch(cbh);
    }

    void ComputePipelineConfigurator::configure(RenderPass *rp, uint32_t subpass) {
        PipelineLayoutManager pipelineLayoutManager = {};

        std::vector<Descriptor *> descriptors = {};
        configureDescriptors(descriptors);

        for (const auto descriptor: descriptors) {
            pipelineLayoutManager.appendDescriptorSetLayout(&descriptor->getLayout());
        }

        configurePushConstant(pipelineLayoutManager);
        PipelineLayout & pipelineLayout = pickFromGlobalRepository<GlobalPipelineLayoutRepo, PipelineLayout &>(
            std::move(pipelineLayoutManager));


        PipelineManager pipelineManager(pipelineLayout, createSpecializationInfo());

        pipelineManager.attachDescriptor(std::move(descriptors));
        pipelineManager.attachShader(&computeShader);

        pipeline = std::make_unique<ComputePipeline>(wc.core, std::move(pipelineManager));
    }

}
