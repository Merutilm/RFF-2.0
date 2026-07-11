//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/manage/PipelineManager.hpp>

namespace merutilm::vkh {
    PipelineManager::PipelineManager(PipelineLayout & layout): layout(layout) {
    }

    void PipelineManager::attachShader(ShaderModule * shaderStage) {
        shaderModules.emplace_back(shaderStage);
    }

    void PipelineManager::attachDescriptor(std::vector<Descriptor *> &&descriptor) { descriptors = std::move(descriptor); }
}