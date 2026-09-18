//
// Created by Merutilm on 2025-05-09.
//

#pragma once
#include <cmath>

#include <numbers>
#include "exponent.hpp"
#include "templates.hpp"

namespace merutilm::rff2::rff_math {


    template<typename Tp>
    concept is_prim = Number<Tp> && (std::is_same_v<Tp, double> || std::is_same_v<Tp, float>);

    template<typename Tp>
    concept is_exponent = Number<Tp> && (std::is_same_v<Tp, dex> || std::is_same_v<Tp, fex>);


    template<Number Num>
    Num abs(Num n) {
        if constexpr (is_prim<Num>) {
            return fabs(n);
        } else {
            static_assert(is_exponent<Num>);
            return n.sgn() == -1 ? -n : n;
        }
    }

    template<Number Num>
    bool is_zero(Num n) {
        if constexpr (is_prim<Num>) {
            return n == 0;
        } else {
            static_assert(is_exponent<Num>);
            return n.is_zero();
        }
    }

    template<Number Num>
    Num try_normalized_value(Num n) {
        if constexpr (is_prim<Num>) {
            return n;
        } else {
            static_assert(is_exponent<Num>);
            n.try_normalize();
            return n;
        }
    }


    template<Number Num>
    Num hypot_approx(Num x, Num y) {
        x = abs(x);
        y = abs(y);
        const Num min = std::min(x, y);
        const Num max = std::max(x, y);

        if (is_zero(min)) {
            return max;
        }
        if (is_zero(max)) {
            return Num(0);
        }

        return max + Num(0.428) * min / max * min;
    }


    inline dex exp(const double v) {

        //e^v
        //exp2 = v / ln2
        //mantissa = decimal value of exp2
        const double raw_exp2 = v / std::numbers::ln2;
        const auto exp2 = static_cast<int>(raw_exp2);
        return dex{exp2, std::exp2(raw_exp2 - exp2)};
    }

    inline dex exp10(const double v) {
        //10 ^ v
        //2 ^ (v * log2(10))
        //exp2 = v * log2(10)
        //mantissa = decimal value of exp2
        const double raw_exp2 = v * std::numbers::ln10 / std::numbers::ln2;
        const auto exp2 = static_cast<int>(raw_exp2);
        return dex{exp2, std::exp2(raw_exp2 - exp2)};
    }




    template<Number Num>
    double log(const Num v) {

        if constexpr (is_prim<Num>) {
            return std::log(v);
        } else {
            static_assert(is_exponent<Num>);

            // w_log(v)
            // = w_log(m * 2^n)
            // = (w_log(m) + w_log(2^n))
            // = (w_log(m) + n * w_log(2))

#ifndef __FINITE_MATH_ONLY__
            if (v.sgn() == -1) {
                return NAN;
            }
            if (v.is_zero()) {
                return -INFINITY;
            }
#endif
            return std::log(v.mantissa) + v.exp2 * std::numbers::ln2;
        }
    }

    template<Number Num>
    double log10(const Num v) {
        // log10(v)
        // = w_log(v) / w_log(10)
#ifndef __FINITE_MATH_ONLY__
        if (v < 0) {
            return NAN;
        }
        if (is_zero(v)) {
            return -INFINITY;
        }
#endif
        return log(v) / std::numbers::ln10;
    }


    template<Number Num>
    int32_t log10Approx(const Num v) {
        if constexpr (is_prim<Num>) {
            const auto bits = std::bit_cast<uint64_t>(static_cast<double>(v));
            const auto rawExp = static_cast<int32_t>((bits >> 52u) & 0x07ffu);
            return static_cast<int32_t>(static_cast<double>(rawExp - 1023) / std::numbers::ln10 * std::numbers::ln2);
        } else {
            static_assert(is_exponent<Num>);
            const auto bits = std::bit_cast<uint64_t>(static_cast<double>(v.mantissa));
            const auto rawExp = static_cast<int32_t>((bits >> 52u) & 0x07ffu);
            return static_cast<int32_t>(static_cast<double>(rawExp - 1023 + v.exp2) / std::numbers::ln10 * std::numbers::ln2);
        }
    }

} // namespace merutilm::rff2::rff_math
