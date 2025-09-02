//
// Created by Merutilm on 2025-07-13.
//

#pragma once

#include "../core/vkh_core.hpp"
#include "HostDataObject.hpp"
#include "../manage/HostDataObjectManager.hpp"

namespace merutilm::vkh {
    class PushConstantImpl final {
        HostDataObject hostDataObject = nullptr;
        VkShaderStageFlags useStage;

    public:
        explicit PushConstantImpl(const VkShaderStageFlags useStage,
                              HostDataObjectManager &&manager) : hostDataObject(factory::create<HostDataObject>(std::move(manager))),
                                                                          useStage(useStage) {
        }

        ~PushConstantImpl() = default;

        PushConstantImpl(const PushConstantImpl &) = delete;

        PushConstantImpl &operator=(const PushConstantImpl &) = delete;

        PushConstantImpl(PushConstantImpl &&) = delete;

        PushConstantImpl &operator=(PushConstantImpl &&) = delete;

        [[nodiscard]] VkShaderStageFlags getUseStage() const {return useStage;}

        [[nodiscard]] HostDataObjectRef getHostObject() const {
            return *hostDataObject;
        }

    };

    using PushConstant = std::unique_ptr<PushConstantImpl>;
    using PushConstantPtr = PushConstantImpl *;
    using PushConstantRef = PushConstantImpl &;
}
