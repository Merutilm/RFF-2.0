//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include "Instance.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDeviceLoader.hpp"

namespace merutilm::vkh {
    class CoreImpl final : public Handler {
        Instance instance = nullptr;
        PhysicalDeviceLoader physicalDevice = nullptr;
        LogicalDevice logicalDevice = nullptr;

        std::chrono::high_resolution_clock::time_point startTime;

    public:
        explicit CoreImpl();

        ~CoreImpl() override;

        CoreImpl(const CoreImpl &) = delete;

        CoreImpl &operator=(const CoreImpl &) = delete;

        CoreImpl(CoreImpl &&) = delete;

        CoreImpl &operator=(CoreImpl &&) = delete;

        [[nodiscard]] InstanceRef getInstance() const { return *instance; }

        [[nodiscard]] PhysicalDeviceLoaderRef getPhysicalDevice() const { return *physicalDevice; }

        [[nodiscard]] LogicalDeviceRef getLogicalDevice() const { return *logicalDevice; }


        [[nodiscard]] float getTime() const;

    private:
        void init() override;

        void destroy() override;
    };

    using Core = std::unique_ptr<CoreImpl>;
    using CorePtr = CoreImpl *;
    using CoreRef = CoreImpl &;
}
