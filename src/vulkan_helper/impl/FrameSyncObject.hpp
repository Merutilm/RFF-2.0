//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class FrameSyncObjectImpl final : public CoreHandler {
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> fences;

    public:
        explicit FrameSyncObjectImpl(CoreRef core);

        ~FrameSyncObjectImpl() override;

        FrameSyncObjectImpl(const FrameSyncObjectImpl &) = delete;

        FrameSyncObjectImpl &operator=(const FrameSyncObjectImpl &) = delete;

        FrameSyncObjectImpl(FrameSyncObjectImpl &&) = delete;

        FrameSyncObjectImpl &operator=(FrameSyncObjectImpl &&) = delete;

        [[nodiscard]] VkSemaphore getImageAvailableSemaphore(const uint32_t frameIndex) const {
            return imageAvailableSemaphores[frameIndex];
        }

        [[nodiscard]] VkSemaphore getRenderFinishedSemaphore(const uint32_t frameIndex) const {
            return renderFinishedSemaphores[frameIndex];
        }

        [[nodiscard]] VkFence getFence(const uint32_t frameIndex) const { return fences[frameIndex]; }

    private:
        void init() override;

        void destroy() override;
    };
    using FrameSyncObject = std::unique_ptr<FrameSyncObjectImpl>;
    using FrameSyncObjectPtr = FrameSyncObjectImpl *;
    using FrameSyncObjectRef = FrameSyncObjectImpl &;
}
