//
// Created by Merutilm on 2025-07-09.
//

#include <vulkan_helper/core/PhysicalDeviceLoader.hpp>

#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/util/PhysicalDeviceUtils.hpp>

#include "vulkan_helper/engine/window/PlatformWindow.hpp"

namespace merutilm::vkh {

    PhysicalDeviceLoader::PhysicalDeviceLoader(Instance &instance) : instance(instance) {
        PhysicalDeviceLoader::init();
    }

    PhysicalDeviceLoader::~PhysicalDeviceLoader() { PhysicalDeviceLoader::cleanup(); }

    VkSurfaceCapabilitiesKHR PhysicalDeviceLoader::populateSurfaceCapabilities(const VkSurfaceKHR surface) const {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
        return surfaceCapabilities;
    }

    void PhysicalDeviceLoader::init() {

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        GLFWwindow *dummyWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        if (!dummyWindow ||
            glfwCreateWindowSurface(instance.getInstanceHandle(), dummyWindow, nullptr, &surface) != VK_SUCCESS) {
            throw exception_init("failed to create dummy window surface");
        }
        initToSuitablePhysicalDevice(surface);
        initToSuitableSurfaceFormat(surface);
        vkDestroySurfaceKHR(instance.getInstanceHandle(), surface, nullptr);
        glfwDestroyWindow(dummyWindow);
        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

        if (physicalDevice == nullptr) {
            throw exception_init("No suitable physical device found");
        }
    }

    void PhysicalDeviceLoader::cleanup() {
        // nothing to do
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


    void PhysicalDeviceLoader::initToSuitableSurfaceFormat(const VkSurfaceKHR surface) {
        uint32_t cnt;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &cnt, nullptr);
        surfaceFormats = std::vector<VkSurfaceFormatKHR>(cnt);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &cnt, surfaceFormats.data());

        std::unordered_set<VkFormat> availableFormats;
        for (const auto &[format, colorSpace]: surfaceFormats) {
            availableFormats.insert(format);
        }
        if (availableFormats.contains(VK_FORMAT_UNDEFINED)) {
            primarySurfaceFormat = VK_FORMAT_R8G8B8A8_UNORM;
            return;
        }

        if (availableFormats.contains(VK_FORMAT_R8G8B8A8_UNORM)) {
            primarySurfaceFormat = VK_FORMAT_R8G8B8A8_UNORM;
            logger::log("Selected Surface Format : VK_FORMAT_R8G8B8A8_UNORM");
            return;
        }

        if (availableFormats.contains(VK_FORMAT_R8G8B8A8_SRGB)) {
            primarySurfaceFormat = VK_FORMAT_R8G8B8A8_SRGB;
            logger::log("Selected Surface Format : VK_FORMAT_R8G8B8A8_SRGB");
            return;
        }

        if (availableFormats.contains(VK_FORMAT_B8G8R8A8_UNORM)) {
            primarySurfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
            logger::log("Selected Surface Format : VK_FORMAT_B8G8R8A8_UNORM");
            return;
        }

        if (availableFormats.contains(VK_FORMAT_B8G8R8A8_SRGB)) {
            primarySurfaceFormat = VK_FORMAT_B8G8R8A8_SRGB;
            logger::log("Selected Surface Format : VK_FORMAT_B8G8R8A8_SRGB");
            return;
        }

        throw exception_init("no suitable surface format are found");
    }
} // namespace merutilm::vkh
