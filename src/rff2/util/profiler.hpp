//
// Created by Merutilm on 2026-04-14.
//

#pragma once
#include <chrono>
#include <unordered_map>
#include <iostream>
#include <x86intrin.h>

namespace merutilm::rff2 {
    struct profiler {
        profiler() = delete;

        static constexpr float CLOCK_GHZ_TEST = 3.8f;

        inline static std::unordered_map<std::string, uint64_t> timers;

        template<typename F>
        static void profile(const std::string &id, F &&func) {
            timers[id] += get_cycles(std::forward<F>(func));
        }

        template<typename F>
        static uint64_t get_cycles(F &&func) {
            unsigned int aux;
            _mm_lfence(); //force finish leading codes
            const auto start = __rdtscp(&aux);
            func(); //measured region
            const auto end = __rdtscp(&aux);
            _mm_lfence(); // prevent trailing codes from leaking into measured region
            return end - start;
        }

        static void print_result() {
            std::stringstream formatted_sec;

            std::cout << "\n=========== Benchmark Result =========== " << std::endl;

            for (auto &[fst, snd]: timers) {
                const float ns = snd / CLOCK_GHZ_TEST;
                if (ns < 1000) {
                    formatted_sec << std::format("{:.2f}", ns) << "ns";
                } else if (ns < 1000000) {
                    formatted_sec << std::format("{:.2f}", ns / 1e3f) << "μs";
                } else if (ns < 1000000000) {
                    formatted_sec << std::format("{:.2f}", ns / 1e6f) << "ms";
                } else {
                    formatted_sec << std::format("{:.2f}", ns / 1e9f) << "s";
                }

                std::cout << fst << " : " << formatted_sec.str() << std::endl;
                formatted_sec.str("");
            }
            timers.clear();
        }
    };
}