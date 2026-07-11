//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/engine/window/Surface.hpp>

#include <vulkan/vulkan_core.h>

namespace merutilm::vkh {
    Surface::Surface(Instance & instance, PlatformWindow * window) : instance(instance), window(window) {
        Surface::init();
    }

    Surface::~Surface() {
        Surface::cleanup();
    }

    void Surface::init() {
        surface = window->createSurface(instance.getInstanceHandle());
    }

    void Surface::cleanup() {
        vkDestroySurfaceKHR(instance.getInstanceHandle(), surface, nullptr);
    }
}
