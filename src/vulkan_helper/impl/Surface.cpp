//
// Created by Merutilm on 2025-07-09.
//

#include "Surface.hpp"

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_win32.h>

#include "../core/vkh_core.hpp"

namespace merutilm::vkh {
    SurfaceImpl::SurfaceImpl(InstanceRef instance, GraphicsContextWindowRef window) : instance(instance), window(window) {
        SurfaceImpl::init();
    }

    SurfaceImpl::~SurfaceImpl() {
        SurfaceImpl::destroy();
    }

    void SurfaceImpl::init() {
        const VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = GetModuleHandle(nullptr),
            .hwnd = window.getWindowHandle()
        };

        if (allocator::invoke(vkCreateWin32SurfaceKHR, instance.getInstanceHandle(), &surfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
            throw exception_init("failed to create window surface!");
        }
    }

    void SurfaceImpl::destroy() {
        allocator::invoke(vkDestroySurfaceKHR, instance.getInstanceHandle(), surface, nullptr);
    }
}
