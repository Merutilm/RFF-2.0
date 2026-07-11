//
// Created by Merutilm on 2025-07-08.
//

#include <vulkan_helper/core/ValidationLayer.hpp>

#include <cstring>
#include <vulkan_helper/base/vkh_base.hpp>
#include <vulkan_helper/util/Debugger.hpp>

namespace merutilm::vkh {
    ValidationLayer::ValidationLayer(const VkInstance instance) : instance(instance) {
        ValidationLayer::init();
    }

    ValidationLayer::~ValidationLayer() {
        ValidationLayer::cleanup();
    }

    void ValidationLayer::init() {
        checkValidationLayerSupport();
        setupDebugMessenger();
    }
    void ValidationLayer::checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        const bool support = std::ranges::any_of(availableLayers, [](const VkLayerProperties &layerProperties) {
            return strcmp(Debugger::VALIDATION_LAYER, layerProperties.layerName) == 0;
        });
        if (!support) {
            throw exception_init("No validation layers available");
        }
    }

    void ValidationLayer::setupDebugMessenger() {

        if (const VkDebugUtilsMessengerCreateInfoEXT createInfo = Debugger::populateDebugMessengerCreateInfo();
            createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw exception_init("Failed to create debug messenger");
        }
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

    void ValidationLayer::cleanup() {
        destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
}