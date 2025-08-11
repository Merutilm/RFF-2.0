//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include <vulkan/vulkan.h>

#include "../def/Engine.hpp"

namespace merutilm::vkh {
    struct Presenter {
        Presenter() = delete;

        static void present(const Engine& engine, const VkSwapchainKHR target, const uint32_t frameIndex, uint32_t imageIndex) {
            VkSwapchainKHR swapchainHandle = target;
            VkSemaphore renderFinishedSemaphore = engine.getSyncObject().getRenderFinishedSemaphore(frameIndex);
            const VkPresentInfoKHR presentInfo = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = nullptr,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &renderFinishedSemaphore,
                .swapchainCount = 1,
                .pSwapchains = &swapchainHandle,
                .pImageIndices = &imageIndex,
                .pResults = nullptr
            };
            if (vkQueuePresentKHR(engine.getCore().getLogicalDevice().getPresentQueue(), &presentInfo) != VK_SUCCESS) {
                throw exception_invalid_state("Failed to present queue");
            }
        }
    };
}
