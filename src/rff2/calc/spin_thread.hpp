//
// Created by Merutilm on 2026-04-26.
//

#pragma once
#include <functional>
#include <thread>
#include <xmmintrin.h>

namespace merutilm::rff2 {

    template<typename... Args>
    class spin_thread {
        std::jthread worker;
        std::function<void(Args...)> func;
        std::tuple<Args...> args;

        std::atomic<bool> run = false;
        std::atomic<bool> stop = false;

    public:
        template<typename F>
            requires std::is_invocable_r_v<void, F, Args...>
        explicit spin_thread(F &&f) : func(std::forward<F>(f)){
            worker = std::jthread([this] {
                while (!stop.load()) {
                    if (run.load(std::memory_order_acquire)) {
                        std::apply(func, args);
                        run.store(false, std::memory_order_release);
                    }
                    _mm_pause();
                }
            });
        }

        ~spin_thread() {
            stop_request();
        }

        spin_thread(const spin_thread &) = delete;

        spin_thread &operator=(const spin_thread &) = delete;

        spin_thread(spin_thread &&) = delete;

        spin_thread &operator=(spin_thread &&) = delete;

        template<typename F>
            requires std::is_invocable_r_v<void, F, Args...>
        void set_function(F &&func) {
            this->func = std::function<void(Args...)>(std::forward<F>(func));
        }

        void run_request(Args &&...args) {
            this->args = std::make_tuple(std::forward<Args>(args)...);
            this->run.store(true, std::memory_order_release);
        }

        void stop_request() {

            this->stop.exchange(true);
        }

        void wait() const {
            while (run.load(std::memory_order_acquire)) {
                _mm_pause();
            }
        };
    };

} // namespace merutilm::rff2
