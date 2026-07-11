//
// Created by Merutilm on 2026-02-03.
//
#include "vulkan_helper/engine/pipeline/Pipeline.hpp"

namespace merutilm::vkh {
    Pipeline::Pipeline(Core &core, PipelineLayout &pipelineLayout, PipelineManager &&pipelineManager) :
        CoreHandler(core), pipelineLayout(pipelineLayout), descriptors(std::move(pipelineManager.descriptors)),
        shaderModules(std::move(pipelineManager.shaderModules)) {}

    Pipeline::~Pipeline() = default;

    std::vector<VkDescriptorSet> Pipeline::enumerateDescriptorSets(const uint32_t frameIndex,
                                                                   DescIndexPicker &&descIndices) const {
        std::vector<VkDescriptorSet> sets(descriptors.size());

        if (!descIndices.empty()) {
            safe_array::check_size_equal(descriptors.size(), descIndices.size(), "Descriptor Index");
        } else {
            descIndices = std::vector<uint32_t>(descriptors.size(), 0);
        }

        for (uint32_t i = 0; i < descriptors.size(); i++) {
            sets[i] = descriptors[i]->getDescriptorSetHandle(frameIndex, descIndices[i]);
        }
        return sets;
    }

    void Pipeline::cleanup() {
        vkDestroyPipeline(core.getLogicalDevice().getLogicalDeviceHandle(), pipeline, nullptr);
    }
} // namespace merutilm::vkh
