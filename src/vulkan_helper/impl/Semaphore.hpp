//
// Created by Merutilm on 2025-09-01.
//

#pragma once
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class SemaphoreImpl final : public CoreHandler {
        VkSemaphore first = VK_NULL_HANDLE;
        VkSemaphore second = VK_NULL_HANDLE;
        VkSemaphore temp = VK_NULL_HANDLE;

    public:
        explicit SemaphoreImpl(CoreRef core);

        ~SemaphoreImpl() override;

        SemaphoreImpl(const SemaphoreImpl &) = delete;

        SemaphoreImpl &operator=(const SemaphoreImpl &) = delete;

        SemaphoreImpl(SemaphoreImpl &&) = delete;

        SemaphoreImpl &operator=(SemaphoreImpl &&) = delete;

        VkSemaphore getFirst() const { return first; }

        VkSemaphore getSecond() const { return second; }

        VkSemaphore getTemp() const { return temp; }

    private:
        void init() override;

        void destroy() override;
    };

    using Semaphore = std::unique_ptr<SemaphoreImpl>;
    using SemaphorePtr = SemaphoreImpl *;
    using SemaphoreRef = SemaphoreImpl &;
}
