//
// Created by Merutilm on 2025-07-13.
//

#include <vulkan_helper/core/Core.hpp>

namespace merutilm::vkh {

    Core::Core() : physicalDevice(instance), logicalDevice(instance, physicalDevice) {
        Core::init();
    }

    Core::~Core() {
        Core::cleanup();
    }

    float Core::getTime() const {
        using namespace std::chrono;
        const time_point<high_resolution_clock> currentTime = high_resolution_clock::now();
        return std::chrono::duration_cast<duration<float> >(currentTime - startTime).count();
    }


    void Core::init() {
        startTime = std::chrono::high_resolution_clock::now();
    }


    void Core::cleanup() {
    }
}
