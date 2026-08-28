//
// Created by Merutilm on 2025-09-01.
//

#pragma once
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class Semaphore final : public CoreHandler {
        VkSemaphore semaphore = VK_NULL_HANDLE;
        VkPipelineStageFlags waitSemaphoreStageMask = 0;

    public:
        explicit Semaphore(Core & core);

        ~Semaphore() override;

        Semaphore(const Semaphore &) = delete;

        Semaphore &operator=(const Semaphore &) = delete;

        Semaphore(Semaphore &&) = delete;

        Semaphore &operator=(Semaphore &&) = delete;

        [[nodiscard]] VkSemaphore getSemaphoreHandle() const { return semaphore; }

        [[nodiscard]] VkPipelineStageFlags getWaitStageMask() const { return waitSemaphoreStageMask; }

        void setWaitStageMask(const VkPipelineStageFlags newMask) {
            waitSemaphoreStageMask = newMask;
        }

    protected:
        void init() override;

        void cleanup() override;
    };


}
