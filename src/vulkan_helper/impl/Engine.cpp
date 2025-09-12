//
// Created by Merutilm on 2025-07-19.
//

#include "Engine.hpp"

#include "../repo/GlobalDescriptorSetLayoutRepo.hpp"
#include "../repo/GlobalPipelineLayoutRepo.hpp"
#include "../repo/GlobalSamplerRepo.hpp"

namespace merutilm::vkh {
    EngineImpl::EngineImpl(Core &&core) : core(std::move(core)) {
        EngineImpl::init();
    }

    EngineImpl::~EngineImpl() {
        EngineImpl::destroy();
    }

    bool EngineImpl::isValidWindowContext(const uint32_t windowAttachmentIndex) const {
        return windowContexts.size() > windowAttachmentIndex && windowContexts[windowAttachmentIndex] != nullptr;
    }

    WindowContextPtr EngineImpl::attachWindowContext(HWND hwnd, uint32_t windowAttachmentIndexExpected) {
        if (windowAttachmentIndexExpected >= windowContexts.size()) {
            windowContexts.resize(windowAttachmentIndexExpected + 1);
        }
        if (windowContexts[windowAttachmentIndexExpected] != nullptr) {
            throw exception_invalid_args(std::format("given window context {} is already using", windowAttachmentIndexExpected));
        }

        auto window = factory::create<GraphicsContextWindow>(hwnd);

        windowContexts[windowAttachmentIndexExpected] = factory::create<WindowContext>(*core, windowAttachmentIndexExpected, std::move(window));
        return windowContexts[windowAttachmentIndexExpected].get();
    }

    void EngineImpl::detachWindowContext(const uint32_t windowAttachmentIndex) {
        if (windowAttachmentIndex >= windowContexts.size()) {
            throw exception_invalid_args(std::format("window attachment index out of range : {}, size = {}", windowAttachmentIndex, windowContexts.size()));
        }
        if (windowContexts[windowAttachmentIndex] == nullptr) {
            throw exception_invalid_args(std::format("deleted non-existing context {}", windowAttachmentIndex));
        }
        windowContexts[windowAttachmentIndex] = nullptr;
    }


    void EngineImpl::init() {
        globalRepositories = factory::create<Repositories>();
        configureRepositories();
    }

    void EngineImpl::configureRepositories() const {
        globalRepositories->addRepository<GlobalDescriptorSetLayoutRepo>(*core);
        globalRepositories->addRepository<GlobalPipelineLayoutRepo>(*core);
        globalRepositories->addRepository<GlobalShaderModuleRepo>(*core);
        globalRepositories->addRepository<GlobalSamplerRepo>(*core);
    }


    void EngineImpl::destroy() {
        windowContexts.clear();
        globalRepositories = nullptr;
        core = nullptr;
    }
}
