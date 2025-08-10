//
// Created by Merutilm on 2025-07-09.
//

#include "Surface.hpp"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

#include "../exception/exception.hpp"

namespace merutilm::mvk {
    Surface::Surface(const Instance &instance, const GraphicsContextWindow &window) : instance(instance), window(window) {
        Surface::init();
    }

    Surface::~Surface() {
        Surface::destroy();
    }

    void Surface::init() {
        const VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = GetModuleHandle(nullptr),
            .hwnd = window.getWindowHandle()
        };

        if (vkCreateWin32SurfaceKHR(instance.getInstanceHandle(), &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
            throw exception_init("failed to create window surface!");
        }
    }

    void Surface::destroy() {
        vkDestroySurfaceKHR(instance.getInstanceHandle(), surface, nullptr);
    }
}
