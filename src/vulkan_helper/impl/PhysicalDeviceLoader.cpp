//
// Created by Merutilm on 2025-07-09.
//

#include "PhysicalDeviceLoader.hpp"

#include "../core/exception.hpp"
#include "../util/PhysicalDeviceUtils.hpp"

namespace merutilm::vkh {
    PhysicalDeviceLoaderImpl::PhysicalDeviceLoaderImpl(InstanceRef instance, SurfaceRef surface) : instance(instance), surface(surface) {
        PhysicalDeviceLoaderImpl::init();
    }

    PhysicalDeviceLoaderImpl::~PhysicalDeviceLoaderImpl() {
        PhysicalDeviceLoaderImpl::destroy();
    }

    VkSurfaceCapabilitiesKHR PhysicalDeviceLoaderImpl::populateSurfaceCapabilities() const {
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface.getSurfaceHandle(), &surfaceCapabilities);
        return surfaceCapabilities;
    }


    void PhysicalDeviceLoaderImpl::init() {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance.getInstanceHandle(), &physicalDeviceCount, nullptr);
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance.getInstanceHandle(), &physicalDeviceCount, physicalDevices.data());

        for (const auto pd: physicalDevices) {
            if (PhysicalDeviceUtils::isDeviceSuitable(pd, surface.getSurfaceHandle())) {
                physicalDevice = pd;
                vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
                vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);
                vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
                queueFamilyIndices = PhysicalDeviceUtils::findQueueFamilies(pd, surface.getSurfaceHandle());
                const VkSurfaceCapabilitiesKHR capabilities = populateSurfaceCapabilities();
                maxFramesInFlight = capabilities.minImageCount + 1;
                if (capabilities.maxImageCount > 0 && maxFramesInFlight > capabilities.maxImageCount) {
                    maxFramesInFlight = capabilities.maxImageCount;
                }
                break;
            }
        }
        if (physicalDevice == nullptr) {
            throw exception_init("No suitable physical device found");
        }
    }



    void PhysicalDeviceLoaderImpl::destroy() {
        //nothing to do
    }
}
