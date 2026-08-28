//
// Created by Merutilm on 2025-08-27.
//

#include <vulkan_helper/engine/pipeline/ComputePipeline.hpp>

namespace merutilm::vkh {
    ComputePipeline::ComputePipeline(Core &core, PipelineManager &&pipelineManager,
                                     std::vector<VkComputePipelineCreateInfo> &&createInfos) :
        Pipeline(core, std::move(pipelineManager)), createInfos(std::move(createInfos)) {
        ComputePipeline::init();
    }

    ComputePipeline::~ComputePipeline() { ComputePipeline::cleanup(); }


    void ComputePipeline::cmdBindAll(const VkCommandBuffer cbh, const uint32_t specializationIndex,
                                     const uint32_t frameIndex, DescIndexPicker &&descIndices) const {
        const auto sets = enumerateDescriptorSets(frameIndex, std::move(descIndices));
        vkCmdBindPipeline(cbh, VK_PIPELINE_BIND_POINT_COMPUTE, pipelines[specializationIndex]);
        if (sets.size() > 0) {
            vkCmdBindDescriptorSets(cbh, VK_PIPELINE_BIND_POINT_COMPUTE, getLayout().getLayoutHandle(), 0,
                                    static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
        }
    }


    void ComputePipeline::init() {
        pipelines.resize(createInfos.size());
        if (vkCreateComputePipelines(core.getLogicalDevice().getLogicalDeviceHandle(), nullptr,
                                     static_cast<uint32_t>(createInfos.size()), createInfos.data(), nullptr,
                                     pipelines.data()) != VK_SUCCESS) {
            throw exception_init("Failed to create compute pipeline!");
        }
        createInfos.clear();
    }
} // namespace merutilm::vkh
