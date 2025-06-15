//
// Created by Merutilm on 2025-06-09.
//

#pragma once
#include <condition_variable>
#include <mutex>
#include <thread>

namespace merutilm::rff {
    class BackgroundThread {
        std::mutex mutex;
        std::condition_variable cv;
        std::jthread thread;

    public:
        template<typename T> requires std::is_invocable_r_v<void, T, BackgroundThread &> && (!std::is_same_v<T, BackgroundThread>)
        explicit BackgroundThread(T &&func);

        template<typename P> requires (!std::is_same_v<P, BackgroundThread>)
        void waitUntil(P &&b);

        void notify();

        ~BackgroundThread() = default;

        BackgroundThread(const BackgroundThread &) = delete;

        BackgroundThread &operator=(const BackgroundThread &) = delete;

        BackgroundThread(BackgroundThread &&) = delete;

        BackgroundThread &operator=(BackgroundThread &&) = delete;

        friend bool operator==(const BackgroundThread &a, const BackgroundThread &b) {
            return &a == &b;
        };
    };


    template<typename T> requires std::is_invocable_r_v<void, T, BackgroundThread &> && (!std::is_same_v<T, BackgroundThread>)
    BackgroundThread::BackgroundThread(T &&func) : thread(std::jthread([this, f = std::forward<T>(func)] { f(*this); })) {
    }

    template<typename P> requires (!std::is_same_v<P, BackgroundThread>)
    void BackgroundThread::waitUntil(P &&b) {
        std::unique_lock lock(mutex);
        cv.wait(lock, std::forward<P>(b));
    }


    inline void BackgroundThread::notify() {
        cv.notify_all();
    }
}