//
// Created by Merutilm on 2025-07-13.
//

#include "Core.hpp"

#include "../core/factory.hpp"
#include "../core/exception.hpp"
#include "../util/PhysicalDeviceUtils.hpp"

namespace merutilm::vkh {

    CoreImpl::CoreImpl() {
        CoreImpl::init();
    }

    CoreImpl::~CoreImpl() {
        CoreImpl::destroy();
    }

    float CoreImpl::getTime() const {
        using namespace std::chrono;
        const auto currentTime = high_resolution_clock::now();
        return std::chrono::duration_cast<duration<float>>(currentTime - startTime).count();
    }


    void CoreImpl::init() {
        instance = factory::create<Instance>();
        startTime = std::chrono::high_resolution_clock::now();
    }

    void CoreImpl::createGraphicsContextForWindow(HWND hwnd, float framerate, const uint32_t graphicsWindowIndexExpected) {
        if (graphicsWindowIndexExpected != windowContexts.size()) {
            throw exception_init(std::format("Window index expected : {} but provided {}", windowContexts.size(), graphicsWindowIndexExpected));
        }

        auto window = factory::create<GraphicsContextWindow>(hwnd, framerate);
        auto surface = factory::create<Surface>(*instance, *window);

        if (physicalDevice == nullptr) {
            physicalDevice = factory::create<PhysicalDeviceLoader>(*instance, *surface);
            logicalDevice = factory::create<LogicalDevice>(*instance, *physicalDevice);
        }else if (!PhysicalDeviceUtils::isDeviceSuitable(physicalDevice->getPhysicalDeviceHandle(), surface->getSurfaceHandle())){
            throw exception_invalid_args("Invalid window provided");
        }

        auto swapchain = factory::create<Swapchain>(*surface, *physicalDevice, *logicalDevice);
        windowContexts.push_back(WindowContext{std::move(window), std::move(surface), std::move(swapchain)});

    }


    void CoreImpl::destroy() {
        windowContexts.clear();
        logicalDevice = nullptr;
        physicalDevice = nullptr;
        instance = nullptr;
    }
}
