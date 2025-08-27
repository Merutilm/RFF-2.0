//
// Created by Merutilm on 2025-07-19.
//

#include "Engine.hpp"

namespace merutilm::vkh {
    EngineImpl::EngineImpl(Core &&core) : core(std::move(core)) {
        EngineImpl::init();
    }

    EngineImpl::~EngineImpl() {
        EngineImpl::destroy();
    }

    void EngineImpl::init() {
        repositories = Factory::create<Repositories>(*core);
        commandPool = Factory::create<CommandPool>(*core);
        commandBuffer = Factory::create<CommandBuffer>(*core, *commandPool);
        syncObject = Factory::create<SyncObject>(*core);
    }



    void EngineImpl::destroy() {
        renderContext.clear();
        syncObject = nullptr;
        commandBuffer = nullptr;
        commandPool = nullptr;
        repositories = nullptr;
        core = nullptr;
    }
}
