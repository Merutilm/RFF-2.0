//
// Created by Merutilm on 2025-07-14.
//

#include "FrameSyncObject.hpp"

#include "../exception/exception.hpp"

namespace merutilm::vkh {
    FrameSyncObjectImpl::FrameSyncObjectImpl(CoreRef core) : CoreHandler(core) {
        FrameSyncObjectImpl::init();
    }

    FrameSyncObjectImpl::~FrameSyncObjectImpl() {
        FrameSyncObjectImpl::destroy();
    }

    void FrameSyncObjectImpl::init() {
        
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();

        constexpr VkSemaphoreCreateInfo semaphoreInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };

        constexpr VkFenceCreateInfo fenceInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };

        imageAvailableSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);
        fences.resize(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS) {
                throw exception_init("Failed to create sync objects!");
            }
        }
    }

    void FrameSyncObjectImpl::destroy() {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, fences[i], nullptr);
        }
    }
}
