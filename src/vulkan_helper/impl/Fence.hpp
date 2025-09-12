//
// Created by Merutilm on 2025-08-29.
//

#pragma once
#include "../core/vkh_core.hpp"
#include "Core.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class FenceImpl final : public CoreHandler {

        VkFence fence = VK_NULL_HANDLE;

    public:
        explicit FenceImpl(CoreRef core);

        ~FenceImpl() override;

        FenceImpl(const FenceImpl &) = delete;

        FenceImpl &operator=(const FenceImpl &) = delete;

        FenceImpl(FenceImpl &&) = delete;

        FenceImpl &operator=(FenceImpl &&) = delete;

        [[nodiscard]] VkFence getFenceHandle() const { return fence; }

        void wait() {
            allocator::invoke(vkWaitForFences, core.getLogicalDevice().getLogicalDeviceHandle(), 1, &fence, VK_TRUE, UINT64_MAX);
        }

        void reset() {
            allocator::invoke(vkResetFences, core.getLogicalDevice().getLogicalDeviceHandle(), 1, &fence);
        }

        void waitAndReset() {
            wait();
            reset();
        }

    private:
        void init() override;

        void destroy() override;
    };

    using Fence = std::unique_ptr<FenceImpl>;
    using FencePtr = FenceImpl *;
    using FenceRef = FenceImpl &;
};
