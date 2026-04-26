//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "dex.h"
#include "dex_std.h"

namespace merutilm::rff2 {
    struct dex_trigonometric {

        explicit dex_trigonometric() = delete;

        static void atan2(dex *result, const dex &y, const dex &x);

        static void sin(dex *result, const dex &v);

        static void cos(dex *result, const dex &v);

        static void tan(dex *result, const dex &v);

        static void hypot_approx(dex *result, dex *temps2, const dex &a, const dex &b);

        static void hypot(dex *result, dex *temp, const dex &a, const dex &b);

        static void hypot2(dex *result, dex *temp, const dex &a, const dex &b);
    };


    inline void dex_trigonometric::atan2(dex *result, const dex &y, const dex &x) {
        const double dv = std::atan2(static_cast<double>(y), static_cast<double>(x));
        dex::cpy(result, y);
        if (dv == 0) {
            return;
        }
        *result /= x;
    }


    inline void dex_trigonometric::sin(dex *result, const dex &v) {
        const double dv = std::sin(static_cast<double>(v));
        if (dv == 0) {
            dex::cpy(result, v);
        }
        dex::cpy(result, dv);
    }


    inline void dex_trigonometric::cos(dex *result, const dex &v) {
        const double dv = std::cos(static_cast<double>(v));
        dex::cpy(result, dv);
    }


    inline void dex_trigonometric::tan(dex *result, const dex &v) {
        const double dv = std::tan(static_cast<double>(v));
        if (dv == 0) {
            dex::cpy(result, v);
        }
        dex::cpy(result, dv);
    }


    inline void dex_trigonometric::hypot_approx(dex *result, dex *temps2, const dex &a, const dex &b) {
        dex_std::abs(result, a);
        dex_std::abs(&temps2[0], b);
        dex_std::max(&temps2[1], temps2[0], *result);
        dex_std::min(&temps2[0], temps2[0], *result);

        if (temps2[0].is_zero()) {
            dex::cpy(result, temps2[1]);
            return;
        }

        if (temps2[1].is_zero()) {
            dex::cpy(result, dex::ZERO);
            return;
        }
        dex::sqr(result, temps2[0]);
        dex::cpy(&temps2[0], 0.428);
        dex::mul(result, *result, temps2[0]);
        dex::div(result, *result, temps2[1]);
        dex::add(result, *result, temps2[1]);
    }

    inline void dex_trigonometric::hypot(dex *result, dex *temp, const dex &a, const dex &b) {
        hypot2(result, temp, a, b);
        dex::sqrt(result, *result);
    }


    inline void dex_trigonometric::hypot2(dex *result, dex *temp, const dex &a, const dex &b) {
        dex::sqr(temp, a);
        dex::sqr(result, b);
        dex::add(result, *result, *temp);
    }

} // namespace merutilm::rff2
