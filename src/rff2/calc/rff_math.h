//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <cmath>
#include <random>

namespace merutilm::rff2 {
    struct rff_math {

        inline static auto rd = std::random_device();
        inline static auto gen = std::mt19937(rd());
        inline static auto urd_i = std::uniform_int_distribution(0, 255);
        inline static auto urd_f = std::uniform_real_distribution(0.0f, 1.0f);
        inline static auto urd_d = std::uniform_real_distribution(0.0, 1.0);


        static double hypot_approx(double x, double y) {
            x = fabs(x);
            y = fabs(y);
            const double min = std::min(x, y);
            const double max = std::max(x, y);

            if (min == 0) {
                return max;
            }
            if (max == 0) {
                return 0;
            }

            return max + 0.428 * min / max * min;
        }
        static float random_i() {
            return urd_i(gen);
        }
        static float random_f() {
            return urd_f(gen);
        }

        static double random_d() {
            return urd_f(gen);
        }
    };
}