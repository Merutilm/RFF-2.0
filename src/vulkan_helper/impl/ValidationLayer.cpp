//
// Created by Merutilm on 2025-07-08.
//

#include "ValidationLayer.hpp"

#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../exception/exception.hpp"
#include "../util/Debugger.hpp"

namespace merutilm::vkh {
    ValidationLayerImpl::ValidationLayerImpl(const VkInstance instance, const bool enabled) : instance(instance),
        enabled(enabled) {
        ValidationLayerImpl::init();
    }

    ValidationLayerImpl::~ValidationLayerImpl() {
        ValidationLayerImpl::destroy();
    }


    void ValidationLayerImpl::checkValidationLayerSupport() const {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        const bool support = std::ranges::any_of(availableLayers, [](const VkLayerProperties &layerProperties) {
            return strcmp(Debugger::VALIDATION_LAYER, layerProperties.layerName) == 0;
        });
        if (enabled && !support) {
            throw exception_init("No validation layers available");
        }
    }


    void ValidationLayerImpl::init() {
        checkValidationLayerSupport();
        setupDebugMessenger();
    }

    void ValidationLayerImpl::setupDebugMessenger() {
        if (!enabled) {
            return;
        }

        if (const VkDebugUtilsMessengerCreateInfoEXT createInfo = Debugger::populateDebugMessengerCreateInfo();
            createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw exception_init("Failed to create debug messenger");
        }
    }


    VkResult ValidationLayerImpl::createDebugUtilsMessengerEXT(const VkInstance instance,
                                                           const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                           const VkAllocationCallbacks *pAllocator,
                                                           VkDebugUtilsMessengerEXT *pDebugMessenger) {
        const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            instance, "vkCreateDebugUtilsMessengerEXT"));
        if (func == nullptr) {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }


    void ValidationLayerImpl::destroyDebugUtilsMessengerEXT(const VkInstance instance,
                                                        const VkDebugUtilsMessengerEXT debugMessenger,
                                                        const VkAllocationCallbacks *pAllocator) {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func == nullptr) {
            return;
        }
        func(instance, debugMessenger, pAllocator);
    }

    void ValidationLayerImpl::destroy() {
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
}
