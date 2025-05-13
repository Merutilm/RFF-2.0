//
// Created by Merutilm on 2025-05-09.
//

#include "ParallelRenderState.h"

#include <mutex>

void ParallelRenderState::createThread(std::function<void(std::stop_token)> &&func) {
    std::scoped_lock lock(mutex);
    if (thread.joinable()) {
        cancelUnsafe();
    }
    thread = std::jthread([f = std::move(func)](const std::stop_token &interrupted) {
        f(interrupted);
    });
}

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
    interrupt();
    thread.join();
}
