//
// Created by Merutilm on 2025-09-07.
//

#include "WindowContext.hpp"

#include "../repo/WindowLocalDescriptorRepo.hpp"
#include "../util/PhysicalDeviceUtils.hpp"

namespace merutilm::vkh {
    WindowContextImpl::WindowContextImpl(CoreRef core, const uint32_t index, GraphicsContextWindow &&window) : CoreHandler(core), attachmentIndex(index),
        window(std::move(window)) {
        WindowContextImpl::init();
    }

    WindowContextImpl::~WindowContextImpl() {
        WindowContextImpl::destroy();
    }

    void WindowContextImpl::init() {

        surface = factory::create<Surface>(core.getInstance(), *window);
        if (!PhysicalDeviceUtils::isDeviceSuitable(core.getPhysicalDevice().getPhysicalDeviceHandle(),
                                                   surface->getSurfaceHandle())) {
            throw exception_invalid_args("Invalid window provided");
        }

        swapchain = factory::create<Swapchain>(core, *surface);
        windowLocalRepositories = factory::create<Repositories>();
        configureRepositories();
        commandPool = factory::create<CommandPool>(core);
        commandBuffer = factory::create<CommandBuffer>(core, *commandPool);
        syncObject = factory::create<SyncObject>(core);
        sharedImageContext = factory::create<SharedImageContext>(core);
    }

    void WindowContextImpl::configureRepositories() const {
        windowLocalRepositories->addRepository<WindowLocalDescriptorRepo>(core);
    }

    void WindowContextImpl::destroy() {
        renderContext.clear();
        sharedImageContext = nullptr;
        syncObject = nullptr;
        commandBuffer = nullptr;
        commandPool = nullptr;
        windowLocalRepositories = nullptr;
        swapchain = nullptr;
        surface = nullptr;
        window = nullptr;
    }
}
