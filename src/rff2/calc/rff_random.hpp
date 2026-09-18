//
// Created by Merutilm on 9/19/26.
//

#pragma once
#include <random>

namespace merutilm::rff2 {
    struct rff_random {
        inline static auto rd = std::random_device();
        inline static auto gen = std::mt19937(rd());
        inline static auto urd_i = std::uniform_int_distribution(0, 255);
        inline static auto urd_f = std::uniform_real_distribution(0.0f, 1.0f);
        inline static auto urd_d = std::uniform_real_distribution(0.0, 1.0);

        static int random_i() { return urd_i(gen); }
        static float random_f() { return urd_f(gen); }
        static double random_d() { return urd_f(gen); }
    };
}