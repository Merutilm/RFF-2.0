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

namespace merutilm::mvk {
    ValidationLayer::ValidationLayer(const VkInstance instance, const bool enabled) : instance(instance),
        enabled(enabled) {
        ValidationLayer::init();
    }

    ValidationLayer::~ValidationLayer() {
        ValidationLayer::destroy();
    }


    void ValidationLayer::checkValidationLayerSupport() const {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        const bool support = std::ranges::any_of(availableLayers, [](const VkLayerProperties &layerProperties) {
            return strcmp(VALIDATION_LAYER, layerProperties.layerName) == 0;
        });
        if (enabled && !support) {
            throw exception_init("No validation layers available");
        }
    }


    VkDebugUtilsMessengerCreateInfoEXT ValidationLayer::populateDebugMessengerCreateInfo() {
        return {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr
        };
    }

    void ValidationLayer::init() {
        checkValidationLayerSupport();
        setupDebugMessenger();
    }

    void ValidationLayer::setupDebugMessenger() {
        if (!enabled) {
            return;
        }

        if (const VkDebugUtilsMessengerCreateInfoEXT createInfo = populateDebugMessengerCreateInfo();
            createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw exception_init("Failed to create debug messenger");
        }
    }


    VkBool32 ValidationLayer::debugCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                   const VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                   [[maybe_unused]] void *pUserData) {
        const char *severityStr = nullptr;
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
                severityStr = "[Verbose]";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
                severityStr = "[Info]";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
                severityStr = "[Warning]";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
                severityStr = "[Error]";
                break;
            }
            default: {
                severityStr = "[Unknown]";
                break;
            }
        }

        const char *messageTypeStr;
        switch (messageType) {
            case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: {
                messageTypeStr = "[General]";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: {
                messageTypeStr = "[Validation]";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: {
                messageTypeStr = "[Performance]";
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT: {
                messageTypeStr = "[DeviceAddressBinding]";
                break;
            }
            default: {
                messageTypeStr = "[Unknown]";
                break;
            }
        }
        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << severityStr << "-" << messageTypeStr << ":" << pCallbackData->pMessage << "\n" <<
                    std::flush;
        }

        return VK_FALSE;
    }

    VkResult ValidationLayer::createDebugUtilsMessengerEXT(const VkInstance instance,
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


    void ValidationLayer::destroyDebugUtilsMessengerEXT(const VkInstance instance,
                                                               const VkDebugUtilsMessengerEXT debugMessenger,
                                                               const VkAllocationCallbacks *pAllocator) {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func == nullptr) {
            return;
        }
        func(instance, debugMessenger, pAllocator);
    }

    void ValidationLayer::destroy() {
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
}
