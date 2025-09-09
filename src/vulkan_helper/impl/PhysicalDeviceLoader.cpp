//
// Created by Merutilm on 2025-07-09.
//

#include "PhysicalDeviceLoader.hpp"

#include "../core/vkh_core.hpp"
#include "../util/PhysicalDeviceUtils.hpp"

namespace merutilm::vkh {

    PhysicalDeviceLoaderImpl::PhysicalDeviceLoaderImpl(InstanceRef instance) : instance(instance) {
        PhysicalDeviceLoaderImpl::init();
    }

    PhysicalDeviceLoaderImpl::~PhysicalDeviceLoaderImpl() {
        PhysicalDeviceLoaderImpl::destroy();
    }

    VkSurfaceCapabilitiesKHR PhysicalDeviceLoaderImpl::populateSurfaceCapabilities(const VkSurfaceKHR surface) const {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
        return surfaceCapabilities;
    }

    HWND PhysicalDeviceLoaderImpl::createDummyWindow() {
        const WNDCLASSEXW wcex = {
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_OWNDC,
            .lpfnWndProc = DefWindowProcW,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = GetModuleHandle(nullptr),
            .hIcon = nullptr,
            .hCursor = nullptr,
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = config::DUMMY_WINDOW_CLASS,
            .hIconSm = nullptr
        };
        RegisterClassExW(&wcex);
        return CreateWindowExW(0, config::DUMMY_WINDOW_CLASS, L"", 0, 0, 0, 100, 100, nullptr, nullptr, nullptr, nullptr);
    }

    VkSurfaceKHR PhysicalDeviceLoaderImpl::createDummySurface(const HWND dummyWindow) const {
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        const VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = nullptr,
            .hwnd = dummyWindow,
        };

        allocator::invoke(vkCreateWin32SurfaceKHR, instance.getInstanceHandle(), &surfaceCreateInfo, nullptr, &surface);

        return surface;
    }

    void PhysicalDeviceLoaderImpl::init() {

        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance.getInstanceHandle(), &physicalDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance.getInstanceHandle(), &physicalDeviceCount, physicalDevices.data());
        const HWND dummyWindow = createDummyWindow();
        const VkSurfaceKHR surface = createDummySurface(dummyWindow);

        for (const auto pd: physicalDevices) {
            if (PhysicalDeviceUtils::isDeviceSuitable(pd, surface)) {
                physicalDevice = pd;
                vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
                vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
                vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
                queueFamilyIndices = PhysicalDeviceUtils::findQueueFamilies(pd, surface);
                const VkSurfaceCapabilitiesKHR capabilities = populateSurfaceCapabilities(surface);
                maxFramesInFlight = capabilities.minImageCount + 1;
                if (capabilities.maxImageCount > 0 && maxFramesInFlight > capabilities.maxImageCount) {
                    maxFramesInFlight = capabilities.maxImageCount;
                }
                break;
            }
        }

        allocator::invoke(vkDestroySurfaceKHR, instance.getInstanceHandle(), surface, nullptr);
        DestroyWindow(dummyWindow);
        UnregisterClassW(config::DUMMY_WINDOW_CLASS, nullptr);

        if (physicalDevice == nullptr) {
            throw exception_init("No suitable physical device found");
        }
    }



    void PhysicalDeviceLoaderImpl::destroy() {
        //nothing to do
    }
}
