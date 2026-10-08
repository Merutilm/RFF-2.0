//
// Created by Merutilm on 2025-07-13.
//

#include <vulkan_helper/core/Core.hpp>

namespace merutilm::vkh {

    Core::Core(const ApplicationCreateInfo &info) : instance(info), physicalDevice(instance), logicalDevice(instance, physicalDevice) {
        Core::init();
    }

    Core::~Core() {
        Core::cleanup();
    }

    void Core::init() {
        //noop
    }


    void Core::cleanup() {
        //noop
    }
}
