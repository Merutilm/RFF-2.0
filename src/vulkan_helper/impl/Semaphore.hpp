//
// Created by Merutilm on 2025-09-01.
//

#pragma once
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class SemaphoreImpl final : public CoreHandler {
        VkSemaphore imageAvailable = VK_NULL_HANDLE;
        VkSemaphore renderFinished = VK_NULL_HANDLE;

    public:
        explicit SemaphoreImpl(CoreRef core);

        ~SemaphoreImpl() override;

        SemaphoreImpl(const SemaphoreImpl &) = delete;

        SemaphoreImpl &operator=(const SemaphoreImpl &) = delete;

        SemaphoreImpl(SemaphoreImpl &&) = delete;

        SemaphoreImpl &operator=(SemaphoreImpl &&) = delete;

        VkSemaphore getImageAvailable() const { return imageAvailable; }

        VkSemaphore getRenderFinished() const { return renderFinished; }

    private:
        void init() override;

        void destroy() override;
    };

    using Semaphore = std::unique_ptr<SemaphoreImpl>;
    using SemaphorePtr = SemaphoreImpl *;
    using SemaphoreRef = SemaphoreImpl &;
}
