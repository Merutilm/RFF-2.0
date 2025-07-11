//
// Created by Merutilm on 2025-06-09.
//

#pragma once
#include <algorithm>
#include <mutex>
#include <list>

#include "BackgroundThread.h"

namespace merutilm::rff2 {
    class BackgroundThreads final {
        std::mutex mutex;
        std::list<bool> ended;
        std::list<BackgroundThread> threads;

    public:
        template<typename T> requires std::is_invocable_r_v<void, T, BackgroundThread &>
        void createThread(T &&func);

        void removeThreadFlag(const BackgroundThread &thread);

        void removeAllEndedThread();

        void notifyAll();
    };

    template<typename T> requires std::is_invocable_r_v<void, T, BackgroundThread &>
    void BackgroundThreads::createThread(T &&func) {
        std::scoped_lock lock(mutex);
        ended.emplace_back(false);
        threads.emplace_back([this, f = std::forward<T>(func)](BackgroundThread &thread) {
            f(thread);
            removeThreadFlag(thread);
        });

    }

    inline void BackgroundThreads::removeThreadFlag(const BackgroundThread &thread) {
        std::scoped_lock lock(mutex);
        auto itThreads = threads.begin();
        auto itEnded = ended.begin();
        while (itThreads != threads.end() && itEnded != ended.end()) {
            if (*itThreads == thread) {
                *itEnded = true;
                return;
            }
            ++itThreads;
            ++itEnded;
        }
    }

    inline void BackgroundThreads::removeAllEndedThread() {
        std::scoped_lock lock(mutex);
        auto itThreads = threads.begin();
        auto itEnded = ended.begin();
        while (itThreads != threads.end() && itEnded != ended.end()) {
            if (*itEnded) {
                itThreads = threads.erase(itThreads);
                itEnded = ended.erase(itEnded);
            } else {
                ++itThreads;
                ++itEnded;
            }
        }
    }

    inline void BackgroundThreads::notifyAll() {
        for (auto &thread : threads) {
            thread.notify();
        }
    }
}