//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/pipeline/PushConstant.hpp>
namespace merutilm::vkh {

    PushConstant::PushConstant(const VkShaderStageFlags useStage, HostDataObjectManager &&manager): hostDataObject(HostDataObject(std::move(manager))),
            useStage(useStage) {
    }

    PushConstant::~PushConstant() = default;

}