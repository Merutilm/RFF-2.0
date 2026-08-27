//
// Created by Merutilm on 2025-07-14.
//

#include <vulkan_helper/engine/sync/SyncObjectGroup.hpp>

namespace merutilm::vkh {
    SyncObjectGroup::SyncObjectGroup(Core & core) : CoreHandler(core) {
        SyncObjectGroup::init();
    }

    SyncObjectGroup::~SyncObjectGroup() {
        SyncObjectGroup::cleanup();
    }

    void SyncObjectGroup::init() {

        const uint32_t maxFramesInFlight = core.getPhysicalDeviceLoader().getMaxFramesInFlight();

        fences.reserve(maxFramesInFlight);
        semaphoreGroups.reserve(maxFramesInFlight);
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            fences.emplace_back(std::make_unique<Fence>(core));
            semaphoreGroups.emplace_back(std::make_unique<SemaphoreGroup>(core));
        }
    }

    void SyncObjectGroup::cleanup() {
        semaphoreGroups.clear();
        fences.clear();
    }
}
