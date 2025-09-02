//
// Created by Merutilm on 2025-07-14.
//

#include "SyncObject.hpp"
#include "../core/factory.hpp"

namespace merutilm::vkh {
    SyncObjectImpl::SyncObjectImpl(CoreRef core) : CoreHandler(core) {
        SyncObjectImpl::init();
    }

    SyncObjectImpl::~SyncObjectImpl() {
        SyncObjectImpl::destroy();
    }

    void SyncObjectImpl::init() {

        const uint32_t maxFramesInFlight = core.getPhysicalDevice().getMaxFramesInFlight();

        fences.resize(maxFramesInFlight);
        semaphores.resize(maxFramesInFlight);
        for (uint32_t i = 0; i < maxFramesInFlight; ++i) {
            fences[i] = factory::create<Fence>(core);
            semaphores[i] = factory::create<Semaphore>(core);
        }
    }

    void SyncObjectImpl::destroy() {
        semaphores.clear();
        fences.clear();
    }
}
