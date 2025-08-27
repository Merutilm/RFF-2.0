//
// Created by Merutilm on 2025-07-14.
//

#include "SyncObject.hpp"

#include "../exception/exception.hpp"

namespace merutilm::vkh {
    SyncObjectImpl::SyncObjectImpl(CoreRef core) : CoreHandler(core) {
        SyncObjectImpl::init();
    }

    SyncObjectImpl::~SyncObjectImpl() {
        SyncObjectImpl::destroy();
    }

    void SyncObjectImpl::init() {
        
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

        imageRenderedSemaphores.resize(maxFramesInFlight);
        renderFinishedSemaphores.resize(maxFramesInFlight);
        fences.resize(maxFramesInFlight);

        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageRenderedSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS) {
                throw exception_init("Failed to create sync objects!");
            }
        }
    }

    void SyncObjectImpl::destroy() {
        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            vkDestroySemaphore(device, imageRenderedSemaphores[i], nullptr);
            vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(device, fences[i], nullptr);
        }
    }
}
