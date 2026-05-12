//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include "Fence.hpp"
#include "Semaphore.hpp"
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class SyncObject final : public CoreHandler {
        std::vector<std::unique_ptr<Fence>> fences = {};
        std::vector<std::unique_ptr<Semaphore>> semaphores = {};


    public:
        explicit SyncObject(Core & core);

        ~SyncObject() override;

        SyncObject(const SyncObject &) = delete;

        SyncObject &operator=(const SyncObject &) = delete;

        SyncObject(SyncObject &&) = delete;

        SyncObject &operator=(SyncObject &&) = delete;

        [[nodiscard]] Semaphore & getSemaphore(const uint32_t frameIndex) const {
            return *semaphores[frameIndex];
        }

        [[nodiscard]] Fence & getFence(const uint32_t frameIndex) const { return *fences[frameIndex]; }

    private:
        void init() override;

        void cleanup() override;
    };


}
