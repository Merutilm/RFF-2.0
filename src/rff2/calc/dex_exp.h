//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "dex.h"


namespace merutilm::rff2 {
    struct dex_exp {

        explicit dex_exp() = delete;

        static void exp10(dex *result, double v);

        static dex exp10(double v);

        static void exp(dex *result, double v);

        static dex exp(double v);

        static double log10(const dex &v);

        static double log(const dex &v);
    };



    inline void dex_exp::exp10(dex *result, const double v) {
        //10 ^ v
        //2 ^ (v * log2(10))
        //exp2 = v / log10(2)
        //mantissa = decimal value of exp2
        const double raw_exp2 = v / Constants::Num::LOG10_2;
        const auto exp2 = static_cast<int>(raw_exp2);
        result->exp2 = exp2;
        result->mantissa = std::pow(2, raw_exp2 - exp2);
        dex::normalize(result);
    }

    inline dex dex_exp::exp10(const double v) {
        dex result = dex::ZERO;
        exp10(&result, v);
        return result;
    }


    inline void dex_exp::exp(dex *result, const double v) {
        //e^v
        //exp2 = v / ln2
        //mantissa = decimal value of exp2
        const double raw_exp2 = v / Constants::Num::LOG_2;
        const auto exp2 = static_cast<int>(raw_exp2);
        result->exp2 = exp2;
        result->mantissa = std::pow(2, raw_exp2 - exp2);
        dex::normalize(result);
    }

    inline dex dex_exp::exp(const double v) {
        dex result = dex::ZERO;
        exp(&result, v);
        return result;
    }


    inline double dex_exp::log10(const dex &v) {
        // log10(v)
        // = w_log(v) / w_log(10)
        if (v.sgn() == -1) {
            return NAN;
        }
        if (v.is_zero()) {
            return -INFINITY;
        }
        return log(v) / Constants::Num::LOG_10;
    }

    inline double dex_exp::log(const dex &v) {
        // w_log(v)
        // = w_log(m * 2^n)
        // = (w_log(m) + w_log(2^n))
        // = (w_log(m) + n * w_log(2))
        if (v.sgn() == -1) {
            return NAN;
        }
        if (v.is_zero()) {
            return -INFINITY;
        }
        return std::log(v.mantissa) + v.exp2 * Constants::Num::LOG_2;
    }
}