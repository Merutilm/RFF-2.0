//
// Created by Merutilm on 2025-07-13.
//

#pragma once
#include <chrono>
#include <memory>

#include "../context/WindowContext.hpp"
#include "../exception/exception.hpp"
#include "Instance.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"

namespace merutilm::vkh {
    class Core final : public Handler {
        std::unique_ptr<Instance> instance = nullptr;
        std::unique_ptr<PhysicalDevice> physicalDevice = nullptr;
        std::unique_ptr<LogicalDevice> logicalDevice = nullptr;

        std::vector<WindowContext> windowContexts = {};

        std::chrono::high_resolution_clock::time_point startTime;

    public:

        explicit Core();

        ~Core() override;

        Core(const Core &) = delete;

        Core &operator=(const Core &) = delete;

        Core(Core &&) = delete;

        Core &operator=(Core &&) = delete;

        void createGraphicsContextForWindow(HWND hwnd, float framerate, uint32_t graphicsWindowIndexExpected);

        [[nodiscard]] const Instance &getInstance() const { return *instance; }

        [[nodiscard]] const PhysicalDevice &getPhysicalDevice() const { return *physicalDevice; }

        [[nodiscard]] const LogicalDevice &getLogicalDevice() const { return *logicalDevice; }

        [[nodiscard]] const WindowContext &getWindowContext(const uint32_t windowIndex) const {
            return windowContexts.at(windowIndex);
        }

        [[nodiscard]] Instance &getInstance() { return *instance; }

        [[nodiscard]] PhysicalDevice &getPhysicalDevice() { return *physicalDevice; }

        [[nodiscard]] LogicalDevice &getLogicalDevice() { return *logicalDevice; }

        [[nodiscard]] WindowContext &getWindowContext(const uint32_t windowIndex) { return windowContexts[windowIndex]; }

        [[nodiscard]] static std::unique_ptr<Core> createCore();

        [[nodiscard]] float getTime() const;

    private:
        void init() override;

        void destroy() override;
    };
}
