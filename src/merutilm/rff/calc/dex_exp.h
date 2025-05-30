//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "double_exp.h"


struct dex_exp {

    explicit dex_exp() = delete;

    static void exp10(double_exp *result, double v);

    static double_exp exp10(double v);

    static void exp(double_exp *result, double v);

    static double_exp exp(double v);

    static double log10(const double_exp &v);

    static double log(const double_exp &v);
};



inline void dex_exp::exp10(double_exp *result, const double v) {
    //10 ^ v
    //2 ^ (v * log2(10))
    //exp2 = v / log10(2)
    //mantissa = decimal value of exp2
    const double raw_exp2 = v / RFF::Constant::LOG10_2;
    const auto exp2 = static_cast<int>(raw_exp2);
    result->exp2 = exp2;
    result->mantissa = std::pow(2, raw_exp2 - exp2);
    double_exp::normalize(result);
}

inline double_exp dex_exp::exp10(const double v) {
    double_exp result = double_exp::DEX_ZERO;
    exp10(&result, v);
    return result;
}


inline void dex_exp::exp(double_exp *result, const double v) {
    //e^v
    //exp2 = v / ln2
    //mantissa = decimal value of exp2
    const double raw_exp2 = v / RFF::Constant::LOG_2;
    const auto exp2 = static_cast<int>(raw_exp2);
    result->exp2 = exp2;
    result->mantissa = std::pow(2, raw_exp2 - exp2);
    double_exp::normalize(result);
}

inline double_exp dex_exp::exp(const double v) {
    double_exp result = double_exp::DEX_ZERO;
    exp(&result, v);
    return result;
}


inline double dex_exp::log10(const double_exp &v) {
    // log10(v)
    // = log(v) / log(10)
    if (v.sgn() == -1) {
        return NAN;
    }
    if (v.is_zero()) {
        return -INFINITY;
    }
    return log(v) / RFF::Constant::LOG_10;
}

inline double dex_exp::log(const double_exp &v) {
    // log(v)
    // = log(m * 2^n)
    // = (log(m) + log(2^n))
    // = (log(m) + n * log(2))
    if (v.sgn() == -1) {
        return NAN;
    }
    if (v.is_zero()) {
        return -INFINITY;
    }
    return std::log(v.mantissa) + v.exp2 * RFF::Constant::LOG_2;
}