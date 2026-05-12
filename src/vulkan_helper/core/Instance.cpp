//
// Created by Merutilm on 2025-07-08.
//

#include <vulkan_helper/core/Instance.hpp>
#include <vulkan_helper/base/config.hpp>
#include <vulkan_helper/base/exception.hpp>
#include <vulkan_helper/util/Debugger.hpp>

namespace merutilm::vkh {

    Instance::Instance() {
        Instance::init();
    }

    Instance::~Instance() {
        Instance::cleanup();
    }

    void Instance::init() {
        createInstance();
        if constexpr(config::ENABLE_VALIDATION) {
            validationLayer = std::make_unique<ValidationLayer>(instance);
        }
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
        if constexpr (config::ENABLE_VALIDATION) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = Debugger::populateDebugMessengerCreateInfo();
        const VkInstanceCreateInfo instanceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = &debugMessengerCreateInfo,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = config::ENABLE_VALIDATION ? 1u : 0,
            .ppEnabledLayerNames =  config::ENABLE_VALIDATION ? &Debugger::VALIDATION_LAYER : nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };

        if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
            throw exception_init("Failed to create instance!");
        }

    }

    void Instance::cleanup() {
        validationLayer = nullptr;
        vkDestroyInstance(instance, nullptr);
    }
}
