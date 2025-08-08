//
// Created by Merutilm on 2025-07-19.
//

#include "Engine.hpp"

namespace merutilm::mvk {
    Engine::Engine(std::unique_ptr<Core> &&core) : core(std::move(core)) {
        Engine::init();
    }

    Engine::~Engine() {
        Engine::destroy();
    }

    void Engine::init() {
        repositories = std::make_unique<Repositories>(*core);
        commandPool = std::make_unique<CommandPool>(*core);
        commandBuffer = std::make_unique<CommandBuffer>(*core, *commandPool);
        syncObject = std::make_unique<SyncObject>(*core);
    }


    void Engine::attachRenderContext(std::unique_ptr<RenderContext> &&renderContext) {
        this->renderContext = std::move(renderContext);
    }

    std::unique_ptr<RenderContext> Engine::detachRenderContext() {
        return std::move(renderContext);
    }

    void Engine::destroy() {
        renderContext = nullptr;
        syncObject = nullptr;
        commandBuffer = nullptr;
        commandPool = nullptr;
        repositories = nullptr;
        core = nullptr;
    }
}
