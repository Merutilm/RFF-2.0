//
// Created by Merutilm on 2025-07-13.
//

#pragma once


#include "../def/HostBufferObject.hpp"
#include "HostBufferObjectManager.hpp"

namespace merutilm::vkh {
    class PushConstantManager : public HostBufferObject {
        VkShaderStageFlags useStage;

    public:
        explicit PushConstantManager(const VkShaderStageFlags useStage,
                              std::unique_ptr<HostBufferObjectManager> &&manager) : HostBufferObject(std::move(manager)),
                                                                          useStage(useStage) {
        }

        ~PushConstantManager() = default;

        PushConstantManager(const PushConstantManager &) = delete;

        PushConstantManager &operator=(const PushConstantManager &) = delete;

        PushConstantManager(PushConstantManager &&) = delete;

        PushConstantManager &operator=(PushConstantManager &&) = delete;

        [[nodiscard]] VkShaderStageFlags getUseStage() const {return useStage;}

    };
}
