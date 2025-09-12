//
// Created by Merutilm on 2025-06-09.
//

#pragma once
#include <mutex>
#include "BackgroundThread.h"

namespace merutilm::rff2 {
    class BackgroundThreads final {
        std::mutex mutex;
        std::vector<std::unique_ptr<BackgroundThread>> threads;

    public:

        template<typename T> requires std::is_invocable_r_v<void, T, BackgroundThread &>
        void createThread(T &&func) {
            std::scoped_lock lock(mutex);

            uint32_t index = threads.size();
            for (uint32_t i = 0; i < threads.size(); ++i) {
                if (threads[i]->isFinished()) {
                    threads[i]->tryJoin();
                    index = i;
                    break;
                }
            }
            if (index == threads.size()) {
                threads.emplace_back(nullptr);
            }
            threads[index] = std::make_unique<BackgroundThread>(std::forward<T>(func));
        }

        void notifyAll() {
            std::scoped_lock lock(mutex);
            for (const auto &thread : threads) {
                thread->notify();
            }
        }
    };




}