//
// Created by Merutilm on 2025-08-29.
//

#include <vulkan_helper/engine/sync/Fence.hpp>


namespace merutilm::vkh {
    Fence::Fence(Core & core) : CoreHandler(core) {
        Fence::init();
    }

    Fence::~Fence() {
        Fence::cleanup();
    }

    void Fence::init() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();

        constexpr VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };


        if (vkCreateFence(device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
            throw exception_init("Failed to create fence!");
        }
    }

    void Fence::cleanup() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkDestroyFence(device, fence, nullptr);
    }
}
