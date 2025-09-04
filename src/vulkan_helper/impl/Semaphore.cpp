//
// Created by Merutilm on 2025-09-01.
//

#include "Semaphore.hpp"

namespace merutilm::vkh {
    SemaphoreImpl::SemaphoreImpl(CoreRef core) : CoreHandler(core) {
        SemaphoreImpl::init();
    }

    SemaphoreImpl::~SemaphoreImpl() {
        SemaphoreImpl::destroy();
    }

    void SemaphoreImpl::init() {

        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();

        constexpr VkSemaphoreCreateInfo semaphoreInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0
        };


        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailable) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinished) != VK_SUCCESS) {
            throw exception_init("Failed to create sync objects!");
        }
    }

    void SemaphoreImpl::destroy() {
        const VkDevice device = core.getLogicalDevice().getLogicalDeviceHandle();
        vkDestroySemaphore(device, imageAvailable, nullptr);
        vkDestroySemaphore(device, renderFinished, nullptr);
    }
}
