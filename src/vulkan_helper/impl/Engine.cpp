//
// Created by Merutilm on 2025-07-19.
//

#include "Engine.hpp"

#include "../util/PhysicalDeviceUtils.hpp"

namespace merutilm::vkh {
    EngineImpl::EngineImpl(Core &&core) : core(std::move(core)) {
        EngineImpl::init();
    }

    EngineImpl::~EngineImpl() {
        EngineImpl::destroy();
    }

    void EngineImpl::createGraphicsContextForWindow(HWND hwnd, float framerate, uint32_t graphicsWindowIndexExpected) {
        if (graphicsWindowIndexExpected != windowContexts.size()) {
            throw exception_init(std::format("Window index expected : {} but provided {}", windowContexts.size(),
                                             graphicsWindowIndexExpected));
        }

        auto window = factory::create<GraphicsContextWindow>(hwnd, framerate);
        auto surface = factory::create<Surface>(core->getInstance(), *window);

        if (!PhysicalDeviceUtils::isDeviceSuitable(core->getPhysicalDevice().getPhysicalDeviceHandle(),
                                                          surface->getSurfaceHandle())) {
            throw exception_invalid_args("Invalid window provided");
        }

        auto swapchain = factory::create<Swapchain>(*core, *surface);
        windowContexts.push_back(WindowContext{std::move(window), std::move(surface), std::move(swapchain)});
    }


    void EngineImpl::init() {
        repositories = factory::create<Repositories>(*core);
        commandPool = factory::create<CommandPool>(*core);
        commandBuffer = factory::create<CommandBuffer>(*core, *commandPool);
        syncObject = factory::create<SyncObject>(*core);
        sharedImageContext = factory::create<SharedImageContext>(*core);
    }


    void EngineImpl::destroy() {
        renderContext.clear();
        windowContexts.clear();
        sharedImageContext = nullptr;
        syncObject = nullptr;
        commandBuffer = nullptr;
        commandPool = nullptr;
        repositories = nullptr;
        core = nullptr;
    }
}
