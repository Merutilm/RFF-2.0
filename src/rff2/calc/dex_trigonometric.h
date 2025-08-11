//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "dex_std.h"
#include "dex.h"

namespace merutilm::rff2 {
    struct dex_trigonometric {

        explicit dex_trigonometric() = delete;

        static void atan2(dex *result, const dex &y, const dex &x);

        static dex atan2(const dex &y, const dex &x);

        static void sin(dex *result, const dex &v);

        static dex sin(const dex &v);

        static void cos(dex *result, const dex &v);

        static dex cos(const dex &v);

        static void tan(dex *result, const dex &v);

        static dex tan(const dex &v);

        static void hypot_approx(dex *result, const dex &a, const dex &b);

        static dex hypot_approx(const dex &a, const dex &b);

        static void hypot(dex *result, const dex &a, const dex &b);

        static dex hypot(const dex &a, const dex &b);

        static void hypot2(dex *result, const dex &a, const dex &b);

        static dex hypot2(const dex &a, const dex &b);
    };


    inline void dex_trigonometric::atan2(dex *result, const dex &y, const dex &x) {
        const double dv = std::atan2(static_cast<double>(y), static_cast<double>(x));
        dex::cpy(result, y);
        if (dv == 0) {
            return;
        }
        *result /= x;
    }

    inline dex dex_trigonometric::atan2(const dex &y, const dex &x) {
        dex result = dex::ZERO;
        atan2(&result, y, x);
        return result;
    }


    inline void dex_trigonometric::sin(dex *result, const dex &v) {
        const double dv = std::sin(static_cast<double>(v));
        if (dv == 0) {
            dex::cpy(result, v);
        }
        dex::cpy(result, dv);
    }


    inline dex dex_trigonometric::sin(const dex &v) {
        dex result = dex::ZERO;
        sin(&result, v);
        return result;
    }

    inline void dex_trigonometric::cos(dex *result, const dex &v) {
        const double dv = std::cos(static_cast<double>(v));
        dex::cpy(result, dv);
    }

    inline dex dex_trigonometric::cos(const dex &v) {
        dex result = dex::ZERO;
        cos(&result, v);
        return result;
    }

    inline void dex_trigonometric::tan(dex *result, const dex &v) {
        const double dv = std::tan(static_cast<double>(v));
        if (dv == 0) {
            dex::cpy(result, v);
        }
        dex::cpy(result, dv);
    }

    inline dex dex_trigonometric::tan(const dex &v) {
        dex result = dex::ZERO;
        tan(&result, v);
        return result;
    }

    inline void dex_trigonometric::hypot_approx(dex *result, const dex &a, const dex &b) {
        const dex a_abs = dex_std::abs(a);
        const dex b_abs = dex_std::abs(b);
        const dex mn = dex_std::min(a_abs, b_abs);
        const dex mx = dex_std::max(a_abs, b_abs);


        if (mn.is_zero()) {
            dex::cpy(result, mx);
            return;
        }

        if (mx.is_zero()) {
            dex::cpy(result, dex::ZERO);
            return;
        }
        dex::sqr(result, mn);
        *result *= 0.428;
        dex::div(result, *result, mx);
        dex::add(result, *result, mx);
    }

    inline dex dex_trigonometric::hypot_approx(const dex &a, const dex &b) {
        dex result = dex::ZERO;
        hypot_approx(&result, a, b);
        return result;
    }

    inline void dex_trigonometric::hypot(dex *result, const dex &a, const dex &b) {
        hypot2(result, a, b);
        dex::sqrt(result, *result);
    }

    inline dex dex_trigonometric::hypot(const dex &a, const dex &b) {
        dex result = dex::ZERO;
        hypot(&result, a, b);
        return result;
    }

    inline void dex_trigonometric::hypot2(dex *result, const dex &a, const dex &b) {
        dex sqr_temp = dex::ZERO;
        dex::sqr(&sqr_temp, a);
        dex::sqr(result, b);
        dex::add(result, *result, sqr_temp);
    }

    inline dex dex_trigonometric::hypot2(const dex &a, const dex &b) {
        dex result = dex::ZERO;
        hypot2(&result, a, b);
        return result;
    }
}