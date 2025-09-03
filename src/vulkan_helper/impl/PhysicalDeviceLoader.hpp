//
// Created by Merutilm on 2025-07-09.
//

#pragma once

#include "Instance.hpp"
#include "Surface.hpp"
#include "../struct/QueueFamilyIndices.hpp"
#include "../handle/Handler.hpp"

namespace merutilm::vkh {
    class PhysicalDeviceLoaderImpl final : public Handler {

        InstanceRef instance;

        VkPhysicalDevice physicalDevice = nullptr;
        VkPhysicalDeviceProperties physicalDeviceProperties = {};
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = {};
        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
        QueueFamilyIndices queueFamilyIndices;
        uint32_t maxFramesInFlight;

    public:

        explicit PhysicalDeviceLoaderImpl(InstanceRef instance);

        ~PhysicalDeviceLoaderImpl() override;

        PhysicalDeviceLoaderImpl(const PhysicalDeviceLoaderImpl &) = delete;

        PhysicalDeviceLoaderImpl &operator=(const PhysicalDeviceLoaderImpl &) = delete;

        PhysicalDeviceLoaderImpl(PhysicalDeviceLoaderImpl &&) = delete;

        PhysicalDeviceLoaderImpl &operator=(PhysicalDeviceLoaderImpl &&) = delete;

        [[nodiscard]] VkPhysicalDevice getPhysicalDeviceHandle() const { return physicalDevice; }

        [[nodiscard]] const VkPhysicalDeviceProperties &getPhysicalDeviceProperties() const {
            return physicalDeviceProperties;
        }

        [[nodiscard]] const VkPhysicalDeviceMemoryProperties &getPhysicalDeviceMemoryProperties() const {
            return physicalDeviceMemoryProperties;
        }

        [[nodiscard]] const VkPhysicalDeviceFeatures &getPhysicalDeviceFeatures() const {
            return physicalDeviceFeatures;
        }

        [[nodiscard]] VkSurfaceCapabilitiesKHR populateSurfaceCapabilities(VkSurfaceKHR surface) const;

        static HWND createDummyWindow();


        [[nodiscard]] const QueueFamilyIndices &getQueueFamilyIndices() const { return queueFamilyIndices; }

        uint32_t getMaxFramesInFlight() const {return maxFramesInFlight;}

    private:

        VkSurfaceKHR createDummySurface(HWND dummyWindow) const;

        void init() override;

        void destroy() override;
    };

    using PhysicalDeviceLoader = std::unique_ptr<PhysicalDeviceLoaderImpl>;
    using PhysicalDeviceLoaderPtr = PhysicalDeviceLoaderImpl *;
    using PhysicalDeviceLoaderRef = PhysicalDeviceLoaderImpl &;
}
