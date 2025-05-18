//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "double_exp.h"
#include "../ui/RFF.h"


struct dex_std {
    explicit dex_std() = delete;

    static void max(double_exp *result, const double_exp &a, const double_exp &b);

    static double_exp max(const double_exp &a, const double_exp &b);

    static void min(double_exp *result, const double_exp &a, const double_exp &b);

    static double_exp min(const double_exp &a, const double_exp &b);

    static void clamp(double_exp *result, const double_exp &target, const double_exp &mn, const double_exp &mx);

    static double_exp clamp(const double_exp &target, const double_exp &mn, const double_exp &mx);

    static void abs(double_exp *result, const double_exp &v);

    static double_exp abs(const double_exp &v);
};


inline void dex_std::max(double_exp *result, const double_exp &a, const double_exp &b) {
    double_exp::dex_cpy(result, a > b ? a : b);
}

inline double_exp dex_std::max(const double_exp &a, const double_exp &b) {
    double_exp result = RFF::Precision::DEX_ZERO;
    max(&result, a, b);
    return result;
}

inline void dex_std::min(double_exp *result, const double_exp &a, const double_exp &b) {
    double_exp::dex_cpy(result, a < b ? a : b);
}

inline double_exp dex_std::min(const double_exp &a, const double_exp &b) {
    double_exp result = RFF::Precision::DEX_ZERO;
    min(&result, a, b);
    return result;
}


inline void dex_std::clamp(double_exp *result, const double_exp &target, const double_exp &mn,
                           const double_exp &mx) {
    min(result, target, mx);
    max(result, mn, target);
}

inline double_exp dex_std::clamp(const double_exp &target, const double_exp &mn, const double_exp &mx) {
    double_exp result = RFF::Precision::DEX_ZERO;
    clamp(&result, target, mn, mx);
    return result;
}


inline void dex_std::abs(double_exp *result, const double_exp &v) {
    double_exp::dex_cpy(result, v < 0 ? -v : v);
}

inline double_exp dex_std::abs(const double_exp &v) {
    double_exp result = RFF::Precision::DEX_ZERO;
    abs(&result, v);
    return result;
}