//
// Created by Merutilm on 2025-08-29.
//

#pragma once
#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/core/Core.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class Fence final : public CoreHandler {

        VkFence fence = VK_NULL_HANDLE;

    public:
        explicit Fence(Core & core);

        ~Fence() override;

        Fence(const Fence &) = delete;

        Fence &operator=(const Fence &) = delete;

        Fence(Fence &&) = delete;

        Fence &operator=(Fence &&) = delete;

        [[nodiscard]] VkFence getFenceHandle() const { return fence; }

        void wait() const {
            vkWaitForFences(core.getLogicalDevice().getLogicalDeviceHandle(), 1, &fence, VK_TRUE, UINT64_MAX);
        }

        void reset() const {
            vkResetFences(core.getLogicalDevice().getLogicalDeviceHandle(), 1, &fence);
        }

        void waitAndReset() const {
            wait();
            reset();
        }

    private:
        void init() override;

        void cleanup() override;
    };


};
