//
// Created by Merutilm on 2025-08-27.
//

#include <vulkan_helper/engine/pipeline/ComputePipeline.hpp>

namespace merutilm::vkh {
    ComputePipeline::ComputePipeline(Core &core, PipelineManager &&pipelineManager) :
        Pipeline(core, std::move(pipelineManager)) {
        ComputePipeline::init();
    }

    ComputePipeline::~ComputePipeline() { ComputePipeline::cleanup(); }


    void ComputePipeline::cmdBindAll(const VkCommandBuffer cbh, const uint32_t specializationIndex, const uint32_t frameIndex,
                                           DescIndexPicker &&descIndices) const {
        const auto sets = enumerateDescriptorSets(frameIndex, std::move(descIndices));
        vkCmdBindPipeline(cbh, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines[specializationIndex]);
        if (sets.size() > 0) {
            vkCmdBindDescriptorSets(cbh, VK_PIPELINE_BIND_POINT_COMPUTE, getLayout().getLayoutHandle(), 0,
                                    static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
        }
    }


    void ComputePipeline::init() {


        std::vector<VkSpecializationInfo> specializationInfo = specialization.buildSpecializationInfo();

        for (uint32_t i = 0; i < specializationInfo.size(); ++i) {
            VkPipeline pipeline = VK_NULL_HANDLE;
            const VkComputePipelineCreateInfo info = {
                    .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .stage = {.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                              .pNext = nullptr,
                              .flags = 0,
                              .stage = VK_SHADER_STAGE_COMPUTE_BIT,
                              .module = getShaderModules()[0]->getShaderModuleHandle(),
                              .pName = "main",
                              .pSpecializationInfo = specialization.isEmpty() ? nullptr : &specializationInfo[i]},
                    .layout = pipelineLayout.getLayoutHandle(),
                    .basePipelineHandle = nullptr,
                    .basePipelineIndex = -1};

            if (vkCreateComputePipelines(core.getLogicalDevice().getLogicalDeviceHandle(), nullptr, 1, &info, nullptr,
                                         &pipeline) != VK_SUCCESS) {
                throw exception_init("Failed to create compute pipeline!");
            }
            pipelines.push_back(pipeline);
        }
    }

} // namespace merutilm::vkh
