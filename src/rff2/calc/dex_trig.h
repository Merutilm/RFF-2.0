//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "dex.h"
#include "dex_std.h"

namespace merutilm::rff2 {
    struct dex_trig {

        explicit dex_trig() = delete;

        static dex atan2(dex y, dex x);

        static dex sin(dex v);

        static dex cos(dex v);

        static dex tan(dex v);

        static dex hypot_approx(dex a, dex b);

        static dex hypot(dex a, dex b);

        static dex hypot2(dex a, dex b);
    };


    inline dex dex_trig::atan2(const dex y, const dex x) {
        const double dv = std::atan2(static_cast<double>(y), static_cast<double>(x));
        return dv == 0 ? y : y / x;
    }


    inline dex dex_trig::sin(const dex v) {
        const double dv = std::sin(static_cast<double>(v));
        return dv == 0 ? v : dex(dv);
    }


    inline dex dex_trig::cos(const dex v) {
        const double dv = std::cos(static_cast<double>(v));
        return dex(dv);
    }


    inline dex dex_trig::tan(const dex v) {
        const double dv = std::tan(static_cast<double>(v));
        return dv == 0 ? v : dex(dv);
    }


    inline dex dex_trig::hypot_approx(const dex a, const dex b) {

        const dex aa = dex_std::abs(a);
        const dex ba = dex_std::abs(b);

        const dex max = dex_std::max(aa, ba);
        const dex min = dex_std::min(aa, ba);

        if (min.is_zero()) {
            return max;
        }

        if (max.is_zero()) {
            return dex::ZERO;
        }

        return min * min * dex(0.428) / max + max;
    }

    inline dex dex_trig::hypot(dex a, dex b) {
        return dex::sqrt(hypot2(a, b));
    }


    inline dex dex_trig::hypot2(dex a, dex b){
        return a * a + b * b;
    }

} // namespace merutilm::rff2
