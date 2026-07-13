//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <functional>
#include <mutex>
#include <thread>

#include "vulkan_helper/base/logger.hpp"

namespace merutilm::rff2 {
    class ParallelRenderState final {
        std::mutex mutex;
        std::jthread thread = std::jthread([](const std::stop_token&) {
            //default empty thread
        });


    public:
        ParallelRenderState() = default;

        template<typename T> requires std::is_invocable_r_v<void, T>
        void createThread(T &&func) {
            std::scoped_lock lock(mutex);

            cancelUnsafe();
            thread = std::jthread([this, f = std::forward<T>(func)]() mutable {
                {
                    //wait until jthread allocation
                    std::scoped_lock lock2(mutex);
                }

                f();
            });
        }

        [[nodiscard]] std::stop_token stopToken() const;

        [[nodiscard]] bool interruptRequested() const;

        void cancel();

        void interrupt();

    private:
        void cancelUnsafe();
    };


}