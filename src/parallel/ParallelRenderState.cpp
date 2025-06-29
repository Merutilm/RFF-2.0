//
// Created by Merutilm on 2025-05-09.
//

#include "ParallelRenderState.h"

#include <future>
#include <mutex>


namespace merutilm::rff2 {
    std::stop_token ParallelRenderState::stopToken() const {
        return thread.get_stop_token();
    }

    bool ParallelRenderState::interruptRequested() const {
        return stopToken().stop_requested();
    }

    void ParallelRenderState::interrupt() {
        thread.request_stop();
    }

    void ParallelRenderState::cancel() {
        std::scoped_lock lock(mutex);
        cancelUnsafe();
    }

    void ParallelRenderState::cancelUnsafe() {
        if (thread.joinable()) {
            interrupt();
            thread.join();
        }
    }
}