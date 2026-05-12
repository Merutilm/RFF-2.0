//
// Created by Merutilm on 2025-07-13.
//

#pragma once

#include <vulkan_helper/engine/buffer/HostDataObject.hpp>
#include <vulkan_helper/engine/manage/HostDataObjectManager.hpp>

namespace merutilm::vkh {
    class PushConstant final {
        HostDataObject hostDataObject;
        VkShaderStageFlags useStage;

    public:
        explicit PushConstant(VkShaderStageFlags useStage,HostDataObjectManager &&manager);

        ~PushConstant();

        PushConstant(const PushConstant &) = delete;

        PushConstant &operator=(const PushConstant &) = delete;

        PushConstant(PushConstant &&) = delete;

        PushConstant &operator=(PushConstant &&) = delete;

        [[nodiscard]] VkShaderStageFlags getUseStage() const {
            return useStage;
        }

        [[nodiscard]] HostDataObject & getHostObject() {
            return hostDataObject;
        }

    };


}
