//
// Created by Merutilm on 2025-07-08.
//

#include "ValidationLayer.hpp"
#include "../core/vkh_core.hpp"
#include "../util/Debugger.hpp"

namespace merutilm::vkh {
    ValidationLayerImpl::ValidationLayerImpl(const VkInstance instance) : instance(instance) {
        ValidationLayerImpl::init();
    }

    ValidationLayerImpl::~ValidationLayerImpl() {
        ValidationLayerImpl::destroy();
    }

    void ValidationLayerImpl::init() {
        checkValidationLayerSupport();
        setupDebugMessenger();
    }
    void ValidationLayerImpl::checkValidationLayerSupport() {
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

    void ValidationLayerImpl::setupDebugMessenger() {

        if (const VkDebugUtilsMessengerCreateInfoEXT createInfo = Debugger::populateDebugMessengerCreateInfo();
            allocator::invoke(createDebugUtilsMessengerEXT, instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
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
        allocator::invoke(destroyDebugUtilsMessengerEXT, instance, debugMessenger, nullptr);
    }
}