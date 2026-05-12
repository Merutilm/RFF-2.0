//
// Created by Merutilm on 2025-07-09.
//

#include <memory>
#include <vulkan_helper/core/PhysicalDeviceLoader.hpp>

#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/util/PhysicalDeviceUtils.hpp>

#include "vulkan_helper/engine/window/PlatformWindowBase.hpp"

namespace merutilm::vkh {

    PhysicalDeviceLoader::PhysicalDeviceLoader(Instance & instance) : instance(instance) {
        PhysicalDeviceLoader::init();
    }

    PhysicalDeviceLoader::~PhysicalDeviceLoader() {
        PhysicalDeviceLoader::cleanup();
    }

    VkSurfaceCapabilitiesKHR PhysicalDeviceLoader::populateSurfaceCapabilities(const VkSurfaceKHR surface) const {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
        return surfaceCapabilities;
    }

    void PhysicalDeviceLoader::init() {



        VkSurfaceKHR surface = VK_NULL_HANDLE;

#ifdef _WIN32

        static constexpr auto dummyClassName = L"VKH_DUMMY";
        const WNDCLASSEX wcex = {
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_OWNDC,
            .lpfnWndProc = DefWindowProc,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = GetModuleHandle(nullptr),
            .hIcon = nullptr,
            .hCursor = nullptr,
            .hbrBackground = nullptr,
            .lpszMenuName = nullptr,
            .lpszClassName = dummyClassName,
            .hIconSm = nullptr
        };
        RegisterClassEx(&wcex);
        const HWND dummyWindow = CreateWindowEx(0, dummyClassName, L"", 0, 0, 0, 100, 100, nullptr, nullptr, nullptr, nullptr);

        const VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .hinstance = nullptr,
            .hwnd = dummyWindow,
        };

        vkCreateWin32SurfaceKHR(instance.getInstanceHandle(), &surfaceCreateInfo, nullptr, &surface);

        initToSuitablePhysicalDevice(surface);
        vkDestroySurfaceKHR(instance.getInstanceHandle(), surface, nullptr);
        UnregisterClassW(config::DUMMY_WINDOW_CLASS, nullptr);

#elif __APPLE__

#elif __linux__

#else
#endif

        if (physicalDevice == nullptr) {
            throw exception_init("No suitable physical device found");
        }
    }


    void PhysicalDeviceLoader::initToSuitablePhysicalDevice(const VkSurfaceKHR surface) {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance.getInstanceHandle(), &physicalDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance.getInstanceHandle(), &physicalDeviceCount, physicalDevices.data());
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
    }


    void PhysicalDeviceLoader::cleanup() {
        //nothing to do
    }
}
