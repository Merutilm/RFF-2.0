//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "dex.h"
#include "dex_std.h"

namespace merutilm::rff2 {
    struct dex_trig {

        explicit dex_trig() = delete;

        static void atan2(dex &result, const dex &y, const dex &x);

        static void sin(dex &result, const dex &v);

        static void cos(dex &result, const dex &v);

        static void tan(dex &result, const dex &v);

        static void hypot_approx(dex &result, dex &tmp0, dex &tmp1, const dex &a, const dex &b);

        static void hypot(dex &result, dex &temp, const dex &a, const dex &b);

        static void hypot2(dex &result, dex &temp, const dex &a, const dex &b);
    };


    inline void dex_trig::atan2(dex &result, const dex &y, const dex &x) {
        const double dv = std::atan2(static_cast<double>(y), static_cast<double>(x));
        dex::cpy(result, y);
        if (dv == 0) {
            return;
        }
        result /= x;
    }


    inline void dex_trig::sin(dex &result, const dex &v) {
        const double dv = std::sin(static_cast<double>(v));
        if (dv == 0) {
            dex::cpy(result, v);
        }
        dex::cpy(result, dv);
    }


    inline void dex_trig::cos(dex &result, const dex &v) {
        const double dv = std::cos(static_cast<double>(v));
        dex::cpy(result, dv);
    }


    inline void dex_trig::tan(dex &result, const dex &v) {
        const double dv = std::tan(static_cast<double>(v));
        if (dv == 0) {
            dex::cpy(result, v);
        }
        dex::cpy(result, dv);
    }


    inline void dex_trig::hypot_approx(dex &result, dex &tmp0, dex &tmp1, const dex &a, const dex &b) {
        dex_std::abs(result, a);
        dex_std::abs(tmp0, b);
        dex_std::max(tmp1, tmp0, result);
        dex_std::min(tmp0, tmp0, result);

        if (tmp0.is_zero()) {
            dex::cpy(result, tmp1);
            return;
        }

        if (tmp1.is_zero()) {
            dex::cpy(result, dex::ZERO);
            return;
        }
        dex::sqr(result, tmp0);
        dex::cpy(tmp0, 0.428);
        dex::mul(result, result, tmp0);
        dex::div(result, result, tmp1);
        dex::add(result, result, tmp1);
    }

    inline void dex_trig::hypot(dex &result, dex &temp, const dex &a, const dex &b) {
        hypot2(result, temp, a, b);
        dex::sqrt(result, result);
    }


    inline void dex_trig::hypot2(dex &result, dex &temp, const dex &a, const dex &b) {
        dex::sqr(temp, a);
        dex::sqr(result, b);
        dex::add(result, result, temp);
    }

} // namespace merutilm::rff2
