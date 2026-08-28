//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include <vulkan_helper/handle/CoreHandler.hpp>
#include "Fence.hpp"
#include "Semaphore.hpp"

namespace merutilm::vkh {
    class SyncObjectGroup final : public CoreHandler {
        std::vector<std::unique_ptr<Fence>> fences = {};
        std::vector<std::unique_ptr<Semaphore>> waitSemaphores = {};
        std::vector<std::unique_ptr<Semaphore>> signalSemaphores = {};


    public:
        explicit SyncObjectGroup(Core & core);

        ~SyncObjectGroup() override;

        SyncObjectGroup(const SyncObjectGroup &) = delete;

        SyncObjectGroup &operator=(const SyncObjectGroup &) = delete;

        SyncObjectGroup(SyncObjectGroup &&) = delete;

        SyncObjectGroup &operator=(SyncObjectGroup &&) = delete;

        [[nodiscard]] Semaphore & getWaitSemaphore(const uint32_t frameIndex) const {
            return *waitSemaphores[frameIndex];
        }
        [[nodiscard]] Semaphore & getSignalSemaphore(const uint32_t frameIndex) const {
            return *signalSemaphores[frameIndex];
        }

        [[nodiscard]] Fence & getFence(const uint32_t frameIndex) const { return *fences[frameIndex]; }

    protected:
        void init() override;

        void cleanup() override;
    };


}
