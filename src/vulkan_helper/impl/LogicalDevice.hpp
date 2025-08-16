//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include <vulkan/vulkan_core.h>

#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "../handle/Handler.hpp"

namespace merutilm::vkh {
    class LogicalDevice final : public Handler {
        const Instance &instance;
        const PhysicalDevice &physicalDevice;
        VkDevice logicalDevice = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

    public:
        explicit LogicalDevice(const Instance &instance, const PhysicalDevice &physicalDevice);

        ~LogicalDevice() override;

        LogicalDevice(const LogicalDevice &) = delete;

        LogicalDevice &operator=(const LogicalDevice &) = delete;

        LogicalDevice(LogicalDevice &&) = delete;

        LogicalDevice &operator=(LogicalDevice &&) = delete;

        [[nodiscard]] VkDevice getLogicalDeviceHandle() const { return logicalDevice; }

        [[nodiscard]] VkQueue getGraphicsQueue() const { return graphicsQueue; }

        [[nodiscard]] VkQueue getPresentQueue() const { return presentQueue; }

    private:
        void init() override;

        void destroy() override;
    };
}
