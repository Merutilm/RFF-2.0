//
// Created by Merutilm on 2025-07-14.
//

#include <vulkan_helper/engine/sync/SyncObject.hpp>

namespace merutilm::vkh {
    SyncObject::SyncObject(Core & core) : CoreHandler(core) {
        SyncObject::init();
    }

    SyncObject::~SyncObject() {
        SyncObject::cleanup();
    }

    void SyncObject::init() {

        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();

        fences.reserve(maxFramesInFlight);
        semaphores.reserve(maxFramesInFlight);
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            fences.emplace_back(std::make_unique<Fence>(core));
            semaphores.emplace_back(std::make_unique<Semaphore>(core));
        }
    }

    void SyncObject::cleanup() {
        semaphores.clear();
        fences.clear();
    }
}
