//
// Created by Merutilm on 2026-02-03.
//

#include <vulkan_helper/util/SwapchainUtils.hpp>


namespace merutilm::vkh {

    void SwapchainUtils::changeFrameIndex(Core &core, uint32_t *frameIndex) {
        ++*frameIndex %= core.getPhysicalDevice().getMaxFramesInFlight();
    }

    uint32_t SwapchainUtils::begin(WindowContext &wc, const uint32_t frameIndex) {
        const Swapchain &swapchain = wc.getSwapchain();
        const VkDevice device = wc.core.getLogicalDevice().getLogicalDeviceHandle();
        const VkSemaphore imageAvailableSemaphore = wc.getSyncObject().getSemaphore(frameIndex).getImageAvailable();
        const VkSwapchainKHR swapchainHandle = swapchain.getSwapchainHandle();


        wc.getSyncObject().getFence(frameIndex).waitAndReset();

        uint32_t swapchainImageIndex = 0;
        vkAcquireNextImageKHR(device, swapchainHandle, UINT64_MAX, imageAvailableSemaphore, nullptr,
                          &swapchainImageIndex);
        return swapchainImageIndex;
    }
    void SwapchainUtils::end(WindowContext &wc, const uint32_t frameIndex, uint32_t swapchainImageIndex) {
        VkSwapchainKHR swapchainHandle = wc.getSwapchain().getSwapchainHandle();
        VkSemaphore renderFinishedSemaphore = wc.getSyncObject().getSemaphore(frameIndex).getRenderFinished();
        const VkPresentInfoKHR presentInfo = {.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                              .pNext = nullptr,
                                              .waitSemaphoreCount = 1,
                                              .pWaitSemaphores = &renderFinishedSemaphore,
                                              .swapchainCount = 1,
                                              .pSwapchains = &swapchainHandle,
                                              .pImageIndices = &swapchainImageIndex,
                                              .pResults = nullptr};
        wc.core.getLogicalDevice().queuePresent(&presentInfo);
    }


} // namespace merutilm::vkh
