//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include <vulkan/vulkan.h>

#include "../impl/Engine.hpp"

namespace merutilm::vkh {
    struct SwapchainPresenter {
        SwapchainPresenter() = delete;

        static void present(EngineRef engine, const VkSwapchainKHR target, const uint32_t frameIndex, uint32_t swapchainImageIndex) {
            VkSwapchainKHR swapchainHandle = target;
            VkSemaphore renderFinishedSemaphore = engine.getSyncObjectBetweenFrame().getRenderFinishedSemaphore(frameIndex);
            const VkPresentInfoKHR presentInfo = {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = nullptr,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = &renderFinishedSemaphore,
                .swapchainCount = 1,
                .pSwapchains = &swapchainHandle,
                .pImageIndices = &swapchainImageIndex,
                .pResults = nullptr
            };
            if (vkQueuePresentKHR(engine.getCore().getLogicalDevice().getPresentQueue(), &presentInfo) != VK_SUCCESS) {
                throw exception_invalid_state("Failed to present queue");
            }
        }
    };
}
