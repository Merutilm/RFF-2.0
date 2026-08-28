//
// Created by Merutilm on 2025-08-27.
//

#include <vulkan_helper/engine/pipeline/GraphicsPipeline.hpp>

namespace merutilm::vkh {
    GraphicsPipeline::GraphicsPipeline(Core &core, PipelineManager &&pipelineManager,
                                       std::vector<VkGraphicsPipelineCreateInfo> &&createInfos) :
        Pipeline(core, std::move(pipelineManager)), createInfos(createInfos) {
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


        pipelines.resize(createInfos.size());
        if (vkCreateGraphicsPipelines(core.getLogicalDevice().getLogicalDeviceHandle(), nullptr,
                                     static_cast<uint32_t>(createInfos.size()), createInfos.data(), nullptr,
                                     pipelines.data()) != VK_SUCCESS) {
            throw exception_init("Failed to create graphics pipeline!");
                                     }
        createInfos.clear();
    }
} // namespace merutilm::vkh
