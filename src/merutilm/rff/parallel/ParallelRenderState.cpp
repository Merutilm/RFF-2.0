//
// Created by Merutilm on 2025-05-09.
//

#include "ParallelRenderState.h"

#include <future>
#include <mutex>


std::stop_token merutilm::rff::ParallelRenderState::stopToken() const {
    return thread.get_stop_token();
}

bool merutilm::rff::ParallelRenderState::interruptRequested() const {
    return stopToken().stop_requested();
}

void merutilm::rff::ParallelRenderState::interrupt() {
    thread.request_stop();
}

void merutilm::rff::ParallelRenderState::cancel() {
    std::scoped_lock lock(mutex);
    cancelUnsafe();
}

void merutilm::rff::ParallelRenderState::cancelUnsafe() {
    if (thread.joinable()) {
        interrupt();
        thread.join();
    }
}
