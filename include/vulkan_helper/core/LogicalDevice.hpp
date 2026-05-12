//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/handle/Handler.hpp>
#include "Instance.hpp"
#include "PhysicalDeviceLoader.hpp"

namespace merutilm::vkh {
    class LogicalDevice final : public Handler {
        Instance &instance;
        PhysicalDeviceLoader &physicalDevice;
        VkDevice logicalDevice = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

        std::mutex mutex;

    public:
        explicit LogicalDevice(Instance &instance, PhysicalDeviceLoader &physicalDevice);

        ~LogicalDevice() override;

        LogicalDevice(const LogicalDevice &) = delete;

        LogicalDevice &operator=(const LogicalDevice &) = delete;

        LogicalDevice(LogicalDevice &&) = delete;

        LogicalDevice &operator=(LogicalDevice &&) = delete;

        [[nodiscard]] VkDevice getLogicalDeviceHandle() const { return logicalDevice; }

        [[nodiscard]] VkQueue getGraphicsQueue() const { return graphicsQueue; }

        [[nodiscard]] VkQueue getPresentQueue() const { return presentQueue; }

        void queueSubmit(const uint32_t submitCount, const VkSubmitInfo *pSubmits, const VkFence fence) {
            std::scoped_lock lock(mutex);
            if (vkQueueSubmit(graphicsQueue, submitCount, pSubmits, fence) != VK_SUCCESS) {
                throw exception_invalid_state("Failed to submit queue!");
            }
        }

        void queuePresent(const VkPresentInfoKHR *presentInfo) {
            std::scoped_lock lock(mutex);
            if (vkQueuePresentKHR(presentQueue, presentInfo) != VK_SUCCESS) {
                throw exception_invalid_state("Failed to present queue!");
            }
        }

        void waitDeviceIdle() {
            std::scoped_lock lock(mutex);
            vkDeviceWaitIdle(logicalDevice);
        }

    private:
        void init() override;

        void cleanup() override;
    };


} // namespace merutilm::vkh
