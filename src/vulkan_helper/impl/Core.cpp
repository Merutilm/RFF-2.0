//
// Created by Merutilm on 2025-07-13.
//

#include "Core.hpp"

#include "../core/factory.hpp"

namespace merutilm::vkh {

    CoreImpl::CoreImpl() {
        CoreImpl::init();
    }

    CoreImpl::~CoreImpl() {
        CoreImpl::destroy();
    }

    float CoreImpl::getTime() const {
        using namespace std::chrono;
        const auto currentTime = high_resolution_clock::now();
        return std::chrono::duration_cast<duration<float>>(currentTime - startTime).count();
    }


    void CoreImpl::init() {
        instance = factory::create<Instance>();
        physicalDevice = factory::create<PhysicalDeviceLoader>(*instance);
        logicalDevice = factory::create<LogicalDevice>(*instance, *physicalDevice);
        startTime = std::chrono::high_resolution_clock::now();
    }


    void CoreImpl::destroy() {
        logicalDevice = nullptr;
        physicalDevice = nullptr;
        instance = nullptr;
    }
}
