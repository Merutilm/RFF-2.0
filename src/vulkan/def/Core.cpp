//
// Created by Merutilm on 2025-07-13.
//

#include "Core.hpp"

#include "../exception/exception.hpp"

namespace merutilm::mvk {

    Core::Core() {
        Core::init();
    }

    Core::~Core() {
        Core::destroy();
    }

    std::unique_ptr<Core> Core::createCore() {
        return std::make_unique<Core>();
    }

    float Core::getTime() const {
        using namespace std::chrono;
        const auto currentTime = high_resolution_clock::now();
        return std::chrono::duration_cast<duration<float>>(currentTime - startTime).count();
    }


    void Core::init() {
        instance = std::make_unique<Instance>(true);
        startTime = std::chrono::high_resolution_clock::now();
    }

    void Core::createGraphicsContextForWindow(HWND hwnd, float framerate, const uint32_t graphicsWindowIndexExpected) {
        if (graphicsWindowIndexExpected != windowContexts.size()) {
            throw exception_init(std::format("Window index expected : {} but provided {}", windowContexts.size(), graphicsWindowIndexExpected));
        }

        auto window = std::make_unique<GraphicsContextWindow>(hwnd, framerate);
        auto surface = std::make_unique<Surface>(*instance, *window);

        if (physicalDevice == nullptr) {
            physicalDevice = std::make_unique<PhysicalDevice>(*instance, *surface);
            logicalDevice = std::make_unique<LogicalDevice>(*instance, *physicalDevice);
        }else if (!PhysicalDevice::isDeviceSuitable(physicalDevice->getPhysicalDeviceHandle(), surface->getSurfaceHandle())){
            throw exception_invalid_args("Invalid window provided");
        }

        auto swapchain = std::make_unique<Swapchain>(*surface, *physicalDevice, *logicalDevice);
        windowContexts.push_back(WindowContext{std::move(window), std::move(surface), std::move(swapchain)});

    }


    void Core::destroy() {
        windowContexts.clear();
        logicalDevice = nullptr;
        physicalDevice = nullptr;
        instance = nullptr;
    }
}
