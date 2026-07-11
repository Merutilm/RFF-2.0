//
// Created by Merutilm on 2025-09-07.
//

#include <vulkan_helper/engine/context/WindowContext.hpp>

#include <vulkan_helper/engine/repo/WindowLocalDescriptorRepo.hpp>
#include <vulkan_helper/util/PhysicalDeviceUtils.hpp>

namespace merutilm::vkh {
    WindowContext::WindowContext(Core &core, CommandPool &commandPool, const uint32_t index, std::unique_ptr<PlatformWindow> &&window) :
        CoreHandler(core), attachmentIndex(index), commandPool(commandPool), window(std::move(window)) {
        WindowContext::init();
    }

    WindowContext::~WindowContext() { WindowContext::cleanup(); }

    void WindowContext::init() {

        surface.emplace(core.getInstance(), window.get());
        if (!PhysicalDeviceUtils::isDeviceSuitable(core.getPhysicalDeviceLoader().getPhysicalDeviceHandle(),
                                                   surface->getSurfaceHandle())) {
            throw exception_invalid_args("Invalid window provided");
        }

        swapchain.emplace(core, *surface);
        windowLocalRepositories.emplace();
        configureRepositories();
        commandBuffer.emplace(core, commandPool);
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
        windowLocalRepositories.reset();
        swapchain.reset();
        surface.reset();
        window.reset();
    }
} // namespace merutilm::vkh
