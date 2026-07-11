//
// Created by Merutilm on 2025-09-01.
//

#pragma once
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class Semaphore final : public CoreHandler {
        VkSemaphore imageAvailable = VK_NULL_HANDLE;
        VkSemaphore renderFinished = VK_NULL_HANDLE;

    public:
        explicit Semaphore(Core & core);

        ~Semaphore() override;

        Semaphore(const Semaphore &) = delete;

        Semaphore &operator=(const Semaphore &) = delete;

        Semaphore(Semaphore &&) = delete;

        Semaphore &operator=(Semaphore &&) = delete;

        VkSemaphore getImageAvailable() const { return imageAvailable; }

        VkSemaphore getRenderFinished() const { return renderFinished; }

    protected:
        void init() override;

        void cleanup() override;
    };


}
