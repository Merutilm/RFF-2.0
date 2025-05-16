//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <functional>
#include <mutex>
#include <thread>


class ParallelRenderState {
    std::mutex mutex;
    std::jthread thread = std::jthread([](std::stop_token){});

public:
    ParallelRenderState() = default;

    template<typename T>
    void createThread(T &&func) {
        std::scoped_lock lock(mutex);
        cancelUnsafe();
        thread = std::jthread([f = std::move(func)](const std::stop_token &interrupted) mutable {
            f(interrupted);
        });
    }

    std::stop_token stopToken() const;

    bool interruptRequested() const;

    void cancel();

    void interrupt();

private:
    void cancelUnsafe();

};
