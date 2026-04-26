//
// Created by Merutilm on 2026-04-25.
//

#pragma once

#include "spin_thread.hpp"

namespace merutilm::rff2 {

    template<typename... Args>
    class spin_thread_pool {
        std::vector<std::unique_ptr<spin_thread<Args...>>> pool;

    public:
        explicit spin_thread_pool() = default;
        ~spin_thread_pool() = default;
        spin_thread_pool(spin_thread_pool const&) = delete;
        spin_thread_pool(spin_thread_pool&&) = delete;
        spin_thread_pool& operator=(spin_thread_pool const&) = delete;
        spin_thread_pool& operator=(spin_thread_pool&&) = delete;

        template<typename F> requires std::is_invocable_r_v<void, F, Args...>
        void add_func(F &&func);

        void run_all(Args&&... args) const;

        void run_ranged(int start, int len, Args&&... args) const;

        void clear_all_task();

        void wait_all() const;
        
        bool is_empty() const;
    };

    
    template<typename... Args>
    template<typename F> requires std::is_invocable_r_v<void, F, Args...>
    void spin_thread_pool<Args...>::add_func(F &&func) {
        pool.emplace_back(std::make_unique<spin_thread<Args...>>(std::forward<F>(func)));
    }


    template<typename... Args>
    void spin_thread_pool<Args...>::run_all(Args&&... args) const {
        for (auto &t: pool) {
            t->run_request(std::forward<Args>(args)...);
        }
    }

    template<typename... Args>
    void spin_thread_pool<Args...>::run_ranged(const int start, const int len, Args&&... args) const {
        for (int i = 0; i < len; ++i) {
            pool[start + i]->run_request(std::forward<Args>(args)...);
        }
    }


    template<typename... Args>
    void spin_thread_pool<Args...>::clear_all_task() {
        pool.clear();
    }

    template<typename... Args>
    void spin_thread_pool<Args...>::wait_all() const {
        for (auto &t: pool) {
            t->wait();
        }
    }


    template<typename... Args>
    bool spin_thread_pool<Args...>::is_empty() const {
        return pool.empty();
    }
}
