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
        //noop
    }


    void EngineImpl::destroy() {
        windowContexts.clear();
        core = nullptr;
    }
}
