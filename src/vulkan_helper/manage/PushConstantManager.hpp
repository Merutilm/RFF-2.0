//
// Created by Merutilm on 2025-07-13.
//

#pragma once


#include "../impl/HostBufferObject.hpp"
#include "HostBufferObjectManager.hpp"

namespace merutilm::vkh {
    class PushConstantManagerImpl : public HostBufferObject {
        VkShaderStageFlags useStage;

    public:
        explicit PushConstantManagerImpl(const VkShaderStageFlags useStage,
                              HostBufferObjectManager &&manager) : HostBufferObject(std::move(manager)),
                                                                          useStage(useStage) {
        }

        ~PushConstantManagerImpl() = default;

        PushConstantManagerImpl(const PushConstantManagerImpl &) = delete;

        PushConstantManagerImpl &operator=(const PushConstantManagerImpl &) = delete;

        PushConstantManagerImpl(PushConstantManagerImpl &&) = delete;

        PushConstantManagerImpl &operator=(PushConstantManagerImpl &&) = delete;

        [[nodiscard]] VkShaderStageFlags getUseStage() const {return useStage;}

    };

    using PushConstantManager = std::unique_ptr<PushConstantManagerImpl>;
    using PushConstantManagerPtr = PushConstantManagerImpl *;
    using PushConstantManagerRef = PushConstantManagerImpl &;
}
