//
// Created by Merutilm on 2025-07-26.
//

#pragma once
#include "../core/vkh.hpp"

namespace merutilm::vkh {
    struct SwapchainUtils {
        SwapchainUtils() = delete;


        template <typename F> requires std::is_invocable_r_v<void, F, uint32_t>
        static void renderFrame(WindowContextRef wc, uint32_t *frameIndex, F&&renderer) {
            if (wc.getWindow().isUnrenderable()) {
                return;
            }
            changeFrameIndex(wc.core, frameIndex);
            auto swapchainImageIndex = begin(wc, *frameIndex);
            renderer(swapchainImageIndex);
            end(wc, *frameIndex, swapchainImageIndex);
        }

        static void changeFrameIndex(CoreRef core, uint32_t *frameIndex) {
            ++*frameIndex %= core.getPhysicalDevice().getMaxFramesInFlight();
        }

        static uint32_t begin(WindowContextRef wc, const uint32_t frameIndex) {
            const SwapchainRef swapchain = wc.getSwapchain();
            const VkDevice device = wc.core.getLogicalDevice().getLogicalDeviceHandle();
            const VkSemaphore imageAvailableSemaphore = wc.getSyncObject().
                    getSemaphore(frameIndex).getImageAvailable();
            const VkSwapchainKHR swapchainHandle = swapchain.getSwapchainHandle();


            wc.getSyncObject().getFence(frameIndex).waitAndReset();

            uint32_t swapchainImageIndex = 0;
            allocator::invoke(vkAcquireNextImageKHR, device, swapchainHandle, UINT64_MAX, imageAvailableSemaphore,
                                  nullptr, &swapchainImageIndex);
            return swapchainImageIndex;
        }

        static void end(WindowContextRef wc, const uint32_t frameIndex, uint32_t swapchainImageIndex) {
            VkSwapchainKHR swapchainHandle = wc.getSwapchain().getSwapchainHandle();
            VkSemaphore renderFinishedSemaphore = wc.getSyncObject().getSemaphore(frameIndex).getRenderFinished();
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
            wc.core.getLogicalDevice().queuePresent(&presentInfo);
        }
    };
}
