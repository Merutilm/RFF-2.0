//
// Created by Merutilm on 2025-06-09.
//

#pragma once
#include <condition_variable>
#include <mutex>
#include <thread>

namespace merutilm::rff2 {
    class BackgroundThread {
        std::mutex mutex;
        std::condition_variable cv;
        std::jthread thread;
        bool finished = false;

    public:


        template<typename T> requires std::is_invocable_r_v<void, T, BackgroundThread &> && (!std::is_same_v<T, BackgroundThread>)
        explicit BackgroundThread(T &&func) : thread(std::jthread([this, f = std::forward<T>(func)] {
            f(*this);
            finished = true;
        })) {
        }
        ~BackgroundThread() = default;

        BackgroundThread(const BackgroundThread &) = delete;

        BackgroundThread &operator=(const BackgroundThread &) = delete;

        BackgroundThread(BackgroundThread &&) = delete;

        BackgroundThread &operator=(BackgroundThread &&) = delete;

        friend bool operator==(const BackgroundThread &a, const BackgroundThread &b) {
            return &a == &b;
        }


        template<typename P> requires (!std::is_same_v<P, BackgroundThread>)
        void waitUntil(P &&b) {
            std::unique_lock lock(mutex);
            cv.wait(lock, std::forward<P>(b));
        }


        void notify() {
            cv.notify_all();
        }

        void tryJoin() {
            if(thread.joinable()) thread.join();
        }

        bool isFinished() const {
            return finished;
        }
    };


}