//
// Created by Merutilm on 2025-08-29.
//

#include "Fence.hpp"


namespace merutilm::vkh {
    FenceImpl::FenceImpl(CoreRef core) : CoreHandler(core) {
        FenceImpl::init();
    }

    FenceImpl::~FenceImpl() {
        FenceImpl::destroy();
    }

    void FenceImpl::init() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();

        constexpr VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };


        if (allocator::invoke(vkCreateFence, device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
            throw exception_init("Failed to create fence!");
        }
    }

    void FenceImpl::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        allocator::invoke(vkDestroyFence, device, fence, nullptr);
    }
}
