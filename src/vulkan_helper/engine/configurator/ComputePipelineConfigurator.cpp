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

        std::vector<VkSpecializationInfo> specializationInfos = pipelineManager.specialization.buildSpecializationInfo();
        std::vector<VkComputePipelineCreateInfo> createInfos;
        createInfos.reserve(specializationInfos.size());

        for (auto &specializationInfo : specializationInfos) {
            const VkComputePipelineCreateInfo info = {
                .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                          .pNext = nullptr,
                          .flags = 0,
                          .stage = VK_SHADER_STAGE_COMPUTE_BIT,
                          .module = computeShader.getShaderModuleHandle(),
                          .pName = "main",
                          .pSpecializationInfo = pipelineManager.specialization.isEmpty() ? nullptr : &specializationInfo},
                .layout = pipelineLayout.getLayoutHandle(),
                .basePipelineHandle = nullptr,
                .basePipelineIndex = -1};
            createInfos.emplace_back(std::move(info));
        }

        pipeline = std::make_unique<ComputePipeline>(wc.core, std::move(pipelineManager), std::move(createInfos));
    }

}
