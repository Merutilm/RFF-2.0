//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <set>

namespace merutilm::rff {
    class ParallelRenderState final {
        std::mutex mutex;
        std::jthread thread = std::jthread([](std::stop_token) {
            //default empty thread
        });


    public:
        ParallelRenderState() = default;

        template<typename T> requires std::is_invocable_r_v<void, T, std::stop_token>
        void createThread(T &&func);

        std::stop_token stopToken() const;

        bool interruptRequested() const;

        void cancel();

        void interrupt();

    private:
        void cancelUnsafe();
    };

    template<typename T> requires std::is_invocable_r_v<void, T, std::stop_token>
    void ParallelRenderState::createThread(T &&func) {
        std::scoped_lock lock(mutex);
        cancelUnsafe();
        thread = std::jthread([f = std::forward<T>(func)](const std::stop_token &interrupted) mutable {
            f(interrupted);
        });
    }
}