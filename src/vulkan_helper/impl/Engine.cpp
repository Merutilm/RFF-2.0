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
        repositories = factory::create<Repositories>(*core);
        commandPool = factory::create<CommandPool>(*core);
        commandBuffer = factory::create<CommandBuffer>(*core, *commandPool);
        syncObjectBetweenFrame = factory::create<SyncObject>(*core);
        syncObjectBetweenRenderPass = factory::create<SyncObject>(*core);
    }



    void EngineImpl::destroy() {
        renderContext.clear();
        syncObjectBetweenRenderPass = nullptr;
        syncObjectBetweenFrame = nullptr;
        commandBuffer = nullptr;
        commandPool = nullptr;
        repositories = nullptr;
        core = nullptr;
    }
}
