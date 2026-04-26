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
        explicit spin_thread(std::function<void(Args...)> f);

        ~spin_thread();

        spin_thread(const spin_thread&) = delete;

        spin_thread& operator=(const spin_thread&) = delete;

        spin_thread(spin_thread&&) = delete;

        spin_thread& operator=(spin_thread&&) = delete;

        template<typename F> requires std::is_invocable_r_v<void, F, Args...>
        void set_function(F &&func);

        void run_request(Args&&... args);

        void stop_request();

        void wait() const;
    };

    template<typename... Args>
    template<typename F> requires std::is_invocable_r_v<void, F, Args...>
    void spin_thread<Args...>::set_function(F &&func) {
        this->func = std::forward<F>(func);
    }

    template<typename... Args>
    void spin_thread<Args...>::run_request(Args&&... args) {
        this->args = std::make_tuple(std::forward<Args>(args)...);
        this->run.store(true, std::memory_order_release);
    }


    template<typename... Args>
    spin_thread<Args...>::spin_thread(std::function<void(Args...)> f) : func(std::move(f)) {
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

    template<typename... Args>
    spin_thread<Args...>::~spin_thread() {
        stop_request();
    }

    template<typename... Args>
    void spin_thread<Args...>::stop_request() {
        this->stop.exchange(true);
    }

    template<typename... Args>
    void spin_thread<Args...>::wait() const {
        while (run.load(std::memory_order_acquire)) {
            _mm_pause();
        }
    }
}
