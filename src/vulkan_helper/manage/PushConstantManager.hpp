//
// Created by Merutilm on 2025-07-13.
//

#pragma once


#include "CompleteBufferObjectManager.hpp"
#include "BufferObjectManager.hpp"

namespace merutilm::vkh {
    class PushConstantManager : public CompleteBufferObjectManager {
        VkShaderStageFlags useStage;

    public:
        explicit PushConstantManager(const VkShaderStageFlags useStage,
                              std::unique_ptr<BufferObjectManager> &&manager) : CompleteBufferObjectManager(std::move(manager)),
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
