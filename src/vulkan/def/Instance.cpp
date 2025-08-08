//
// Created by Merutilm on 2025-07-08.
//

#include "Instance.hpp"

#include "../exception/exception.hpp"

namespace merutilm::mvk {

    Instance::Instance(const bool enableValidationLayer) : enableValidationLayer(enableValidationLayer){
        Instance::init();
    }

    Instance::~Instance() {
        Instance::destroy();
    }


    void Instance::init() {
        createInstance();
        validationLayer = std::make_unique<ValidationLayer>(instance, enableValidationLayer);
    }


    void Instance::createInstance() {
        VkApplicationInfo applicationInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "Application",
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "1.0.0",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_0,
        };
        if (enableValidationLayer) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = ValidationLayer::populateDebugMessengerCreateInfo();

        const VkInstanceCreateInfo instanceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = enableValidationLayer ? &debugMessengerCreateInfo : nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = enableValidationLayer ? static_cast<uint32_t>(1) : 0,
            .ppEnabledLayerNames = enableValidationLayer ? &ValidationLayer::VALIDATION_LAYER : nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };
        if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
            throw exception_init("Failed to create instance!");
        }

    }

    void Instance::destroy() {
        validationLayer = nullptr;
        vkDestroyInstance(instance, nullptr);
    }
}
