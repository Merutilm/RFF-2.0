//
// Created by Merutilm on 2025-07-08.
//

#include "Instance.hpp"

#include "../def/Factory.hpp"
#include "../exception/exception.hpp"
#include "../util/Debugger.hpp"

namespace merutilm::vkh {

    InstanceImpl::InstanceImpl(const bool enableValidationLayer) : enableValidationLayer(enableValidationLayer){
        InstanceImpl::init();
    }

    InstanceImpl::~InstanceImpl() {
        InstanceImpl::destroy();
    }


    void InstanceImpl::init() {
        createInstance();
        validationLayer = Factory::create<ValidationLayer>(instance, enableValidationLayer);
    }


    void InstanceImpl::createInstance() {
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
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = Debugger::populateDebugMessengerCreateInfo();

        const VkInstanceCreateInfo instanceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = enableValidationLayer ? &debugMessengerCreateInfo : nullptr,
            .flags = 0,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = enableValidationLayer ? static_cast<uint32_t>(1) : 0,
            .ppEnabledLayerNames = enableValidationLayer ? &Debugger::VALIDATION_LAYER : nullptr,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };
        if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance)) {
            throw exception_init("Failed to create instance!");
        }

    }

    void InstanceImpl::destroy() {
        validationLayer = nullptr;
        vkDestroyInstance(instance, nullptr);
    }
}
