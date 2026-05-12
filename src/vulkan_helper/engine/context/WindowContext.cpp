//
// Created by Merutilm on 2025-09-07.
//

#include <vulkan_helper/engine/context/WindowContext.hpp>

#include <vulkan_helper/engine/repo/WindowLocalDescriptorRepo.hpp>
#include <vulkan_helper/util/PhysicalDeviceUtils.hpp>

namespace merutilm::vkh {
    WindowContext::WindowContext(Core &core, const uint32_t index, std::unique_ptr<PlatformWindowBase> &&window) :
        CoreHandler(core), attachmentIndex(index), window(std::move(window)) {
        WindowContext::init();
    }

    WindowContext::~WindowContext() { WindowContext::cleanup(); }

    void WindowContext::init() {

        surface.emplace(core.getInstance(), window.get());
        if (!PhysicalDeviceUtils::isDeviceSuitable(core.getPhysicalDevice().getPhysicalDeviceHandle(),
                                                   surface->getSurfaceHandle())) {
            throw exception_invalid_args("Invalid window provided");
        }

        swapchain.emplace(core, *surface);
        windowLocalRepositories.emplace();
        configureRepositories();
        commandPool.emplace(core);
        commandBuffer.emplace(core, *commandPool);
        syncObject.emplace(core);
        sharedImageContext.emplace(core);
    }

    void WindowContext::configureRepositories() {
        windowLocalRepositories->addRepository<WindowLocalDescriptorRepo>(core);
    }

    void WindowContext::cleanup() {
        renderContexts.clear();
        sharedImageContext.reset();
        syncObject.reset();
        commandBuffer.reset();
        commandPool.reset();
        windowLocalRepositories.reset();
        swapchain.reset();
        surface.reset();
        window.reset();
    }
} // namespace merutilm::vkh
