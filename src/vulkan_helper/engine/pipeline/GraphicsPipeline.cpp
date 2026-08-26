//
// Created by Merutilm on 2025-08-27.
//

#include <vulkan_helper/engine/pipeline/GraphicsPipeline.hpp>

namespace merutilm::vkh {
    GraphicsPipeline::GraphicsPipeline(Core &core, PipelineManager &&pipelineManager,
                                       std::vector<VkGraphicsPipelineCreateInfo> &&createInfo) :
        Pipeline(core, std::move(pipelineManager)), createInfo(createInfo) {
        GraphicsPipeline::init();
    }

    GraphicsPipeline::~GraphicsPipeline() { GraphicsPipeline::cleanup(); }

    void GraphicsPipeline::cmdBindAll(const VkCommandBuffer cbh, const uint32_t specializationIndex,
                                      const uint32_t frameIndex, DescIndexPicker &&descIndices) const {
        const auto sets = enumerateDescriptorSets(frameIndex, std::move(descIndices));
        vkCmdBindPipeline(cbh, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[specializationIndex]);
        if (!sets.empty()) {
            vkCmdBindDescriptorSets(cbh, VK_PIPELINE_BIND_POINT_GRAPHICS, getLayout().getLayoutHandle(), 0,
                                    static_cast<uint32_t>(sets.size()), sets.data(), 0, nullptr);
        }
    }


    void GraphicsPipeline::init() {

        const uint32_t cnt = std::max(static_cast<uint32_t>(specialization.entries.size()), static_cast<uint32_t>(1));

        for (uint32_t i = 0; i < cnt; ++i) {
            VkPipeline pipeline = VK_NULL_HANDLE;
            if (vkCreateGraphicsPipelines(core.getLogicalDevice().getLogicalDeviceHandle(), nullptr, 1, &createInfo[i],
                                          nullptr, &pipeline) != VK_SUCCESS) {
                throw exception_init("Failed to create graphics pipeline!");
            }
            pipelines.push_back(pipeline);
        }
    }
} // namespace merutilm::vkh
