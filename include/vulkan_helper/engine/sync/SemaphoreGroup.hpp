//
// Created by Merutilm on 2025-09-01.
//

#pragma once
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class SemaphoreGroup final : public CoreHandler {
        VkSemaphore imageAvailable = VK_NULL_HANDLE;
        VkSemaphore renderFinished = VK_NULL_HANDLE;

    public:
        explicit SemaphoreGroup(Core & core);

        ~SemaphoreGroup() override;

        SemaphoreGroup(const SemaphoreGroup &) = delete;

        SemaphoreGroup &operator=(const SemaphoreGroup &) = delete;

        SemaphoreGroup(SemaphoreGroup &&) = delete;

        SemaphoreGroup &operator=(SemaphoreGroup &&) = delete;

        [[nodiscard]] VkSemaphore getImageAvailable() const { return imageAvailable; }

        [[nodiscard]] VkSemaphore getRenderFinished() const { return renderFinished; }

    protected:
        void init() override;

        void cleanup() override;
    };


}
