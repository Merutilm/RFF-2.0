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

        std::mutex queueMutex;

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

        std::mutex &getQueueMutex() { return queueMutex; }

        void queueSubmit(uint32_t submitCount, const VkSubmitInfo *pSubmits, VkFence fence);

        void queuePresent(const VkPresentInfoKHR *presentInfo);

        void waitDeviceIdle();

    protected:
        void init() override;

        void cleanup() override;
    };


} // namespace merutilm::vkh
