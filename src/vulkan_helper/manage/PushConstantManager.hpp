//
// Created by Merutilm on 2025-07-13.
//

#pragma once


#include "CompleteShaderObjectManager.hpp"
#include "ShaderObjectManager.hpp"

namespace merutilm::vkh {
    class PushConstantManager : public CompleteShaderObjectManager {
        VkShaderStageFlags useStage;

    public:
        explicit PushConstantManager(const VkShaderStageFlags useStage,
                              std::unique_ptr<ShaderObjectManager> &&manager) : CompleteShaderObjectManager(std::move(manager)),
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
