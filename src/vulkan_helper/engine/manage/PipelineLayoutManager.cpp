//
// Created by Merutilm on 2026-02-03.
//

#include <vulkan_helper/engine/manage/PipelineLayoutManager.hpp>
namespace merutilm::vkh {
    
    PipelineLayoutManager::PipelineLayoutManager() = default;

    PipelineLayoutManager::~PipelineLayoutManager() = default;

    bool PipelineLayoutManager::operator==(const PipelineLayoutManager &) const = default;

    void PipelineLayoutManager::appendDescriptorSetLayout(DescriptorSetLayout * descriptorSetLayout) {
        builders.emplace_back(descriptorSetLayout);
        ++descriptorSetLayoutCount;
    }

    void PipelineLayoutManager::appendPushConstantManager(PushConstant * pushConstant) {
        builders.emplace_back(pushConstant);
    }
}