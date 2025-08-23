//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class SyncObject final : public CoreHandler {
        std::vector<VkSemaphore> imageRenderedSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> fences;

    public:
        explicit SyncObject(const CoreRef core);

        ~SyncObject() override;

        SyncObject(const SyncObject &) = delete;

        SyncObject &operator=(const SyncObject &) = delete;

        SyncObject(SyncObject &&) = delete;

        SyncObject &operator=(SyncObject &&) = delete;

        [[nodiscard]] VkSemaphore getImageAvailableSemaphore(const uint32_t frameIndex) const {
            return imageRenderedSemaphores[frameIndex];
        }

        [[nodiscard]] VkSemaphore getRenderFinishedSemaphore(const uint32_t frameIndex) const {
            return renderFinishedSemaphores[frameIndex];
        }

        [[nodiscard]] VkFence getFence(const uint32_t frameIndex) const { return fences[frameIndex]; }

    private:
        void init() override;

        void destroy() override;
    };
}
