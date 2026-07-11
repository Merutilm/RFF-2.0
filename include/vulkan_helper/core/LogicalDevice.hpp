//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan_helper/handle/Handler.hpp>
#include "Instance.hpp"
#include "PhysicalDeviceLoader.hpp"
#include "vulkan_helper/base/logger.hpp"

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
            VkResult result;
            if ((result = vkQueueSubmit(graphicsQueue, submitCount, pSubmits, fence)) != VK_SUCCESS) {
                throw exception_invalid_state("Failed to submit queue! "  + std::to_string(result));
            }
        }

        void queuePresent(const VkPresentInfoKHR *presentInfo) {
            std::scoped_lock lock(mutex);
            const VkResult result = vkQueuePresentKHR(presentQueue, presentInfo);
            if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
                logger::log("warning : the surface is suboptimal or out of date. your window resizing request is so fast!");
            }
            else if (result != VK_SUCCESS) {
                throw exception_invalid_state("Failed to present queue! " + std::to_string(result));
            }
        }

        void waitDeviceIdle() {
            std::scoped_lock lock(mutex);
            vkDeviceWaitIdle(logicalDevice);
        }

    protected:
        void init() override;

        void cleanup() override;
    };


} // namespace merutilm::vkh
