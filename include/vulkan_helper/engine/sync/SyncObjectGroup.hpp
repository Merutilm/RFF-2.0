//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include <vulkan_helper/handle/CoreHandler.hpp>
#include "Fence.hpp"
#include "SemaphoreGroup.hpp"

namespace merutilm::vkh {
    class SyncObjectGroup final : public CoreHandler {
        std::vector<std::unique_ptr<Fence>> fences = {};
        std::vector<std::unique_ptr<SemaphoreGroup>> semaphoreGroups = {};


    public:
        explicit SyncObjectGroup(Core & core);

        ~SyncObjectGroup() override;

        SyncObjectGroup(const SyncObjectGroup &) = delete;

        SyncObjectGroup &operator=(const SyncObjectGroup &) = delete;

        SyncObjectGroup(SyncObjectGroup &&) = delete;

        SyncObjectGroup &operator=(SyncObjectGroup &&) = delete;

        [[nodiscard]] SemaphoreGroup & getSemaphore(const uint32_t frameIndex) const {
            return *semaphoreGroups[frameIndex];
        }

        [[nodiscard]] Fence & getFence(const uint32_t frameIndex) const { return *fences[frameIndex]; }

    protected:
        void init() override;

        void cleanup() override;
    };


}
