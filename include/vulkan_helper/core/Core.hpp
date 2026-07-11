//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include "Instance.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDeviceLoader.hpp"

namespace merutilm::vkh {
    class Core final : public Handler {
        Instance instance;
        PhysicalDeviceLoader physicalDevice;
        LogicalDevice logicalDevice;

        std::chrono::high_resolution_clock::time_point startTime;

    public:
        explicit Core();

        ~Core() override;

        Core(const Core &) = delete;

        Core &operator=(const Core &) = delete;

        Core(Core &&) = delete;

        Core &operator=(Core &&) = delete;

        [[nodiscard]] Instance & getInstance() { return instance; }

        [[nodiscard]] PhysicalDeviceLoader & getPhysicalDeviceLoader() { return physicalDevice; }

        [[nodiscard]] LogicalDevice & getLogicalDevice() { return logicalDevice; }


        [[nodiscard]] float getTime() const;

    protected:
        void init() override;

        void cleanup() override;
    };


}
