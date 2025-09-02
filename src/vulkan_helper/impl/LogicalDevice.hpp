//
// Created by Merutilm on 2025-07-09.
//

#pragma once
#include "Instance.hpp"
#include "PhysicalDeviceLoader.hpp"
#include "../handle/Handler.hpp"

namespace merutilm::vkh {
    class LogicalDeviceImpl final : public Handler {
        InstanceRef instance;
        PhysicalDeviceLoaderRef physicalDevice;
        VkDevice logicalDevice = nullptr;
        VkQueue graphicsQueue = nullptr;
        VkQueue presentQueue = nullptr;

    public:
        explicit LogicalDeviceImpl(InstanceRef instance, PhysicalDeviceLoaderRef physicalDevice);

        ~LogicalDeviceImpl() override;

        LogicalDeviceImpl(const LogicalDeviceImpl &) = delete;

        LogicalDeviceImpl &operator=(const LogicalDeviceImpl &) = delete;

        LogicalDeviceImpl(LogicalDeviceImpl &&) = delete;

        LogicalDeviceImpl &operator=(LogicalDeviceImpl &&) = delete;

        [[nodiscard]] VkDevice getLogicalDeviceHandle() const { return logicalDevice; }

        [[nodiscard]] VkQueue getGraphicsQueue() const { return graphicsQueue; }

        [[nodiscard]] VkQueue getPresentQueue() const { return presentQueue; }

    private:
        void init() override;

        void destroy() override;
    };

    using LogicalDevice = std::unique_ptr<LogicalDeviceImpl>;
    using LogicalDevicePtr = LogicalDeviceImpl *;
    using LogicalDeviceRef = LogicalDeviceImpl &;
}
