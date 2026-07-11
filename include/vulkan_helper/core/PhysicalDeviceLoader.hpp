//
// Created by Merutilm on 2025-07-09.
//

#pragma once

#include <vulkan_helper/handle/Handler.hpp>
#include "Instance.hpp"
#include "QueueFamilyIndices.hpp"

namespace merutilm::vkh {
    class PhysicalDeviceLoader final : public Handler {

        Instance & instance;

        VkPhysicalDevice physicalDevice = nullptr;
        VkPhysicalDeviceProperties physicalDeviceProperties = {};
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = {};
        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
        QueueFamilyIndices queueFamilyIndices;
        std::vector<VkSurfaceFormatKHR> surfaceFormats;
        VkFormat primarySurfaceFormat;
        uint32_t maxFramesInFlight;

    public:

        explicit PhysicalDeviceLoader(Instance & instance);

        ~PhysicalDeviceLoader() override;

        PhysicalDeviceLoader(const PhysicalDeviceLoader &) = delete;

        PhysicalDeviceLoader &operator=(const PhysicalDeviceLoader &) = delete;

        PhysicalDeviceLoader(PhysicalDeviceLoader &&) = delete;

        PhysicalDeviceLoader &operator=(PhysicalDeviceLoader &&) = delete;

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


        [[nodiscard]] const QueueFamilyIndices &getQueueFamilyIndices() const { return queueFamilyIndices; }

        [[nodiscard]] uint32_t getMaxFramesInFlight() const {return maxFramesInFlight;}

        [[nodiscard]] const std::vector<VkSurfaceFormatKHR> &getSupportedSurfaceFormats() const {
            return surfaceFormats;
        }

        [[nodiscard]] VkFormat getPrimarySurfaceFormat() const {
            return primarySurfaceFormat;
        }


    protected:
        void init() override;

        void cleanup() override;

    private:
        void initToSuitableSurfaceFormat(VkSurfaceKHR surface);

        void initToSuitablePhysicalDevice(VkSurfaceKHR surface);

    };


}
