//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "dex_std.h"
#include "double_exp.h"

struct dex_trigonometric {

    explicit dex_trigonometric() = delete;

    static void atan2(double_exp *result, const double_exp &y, const double_exp &x);

    static double_exp atan2(const double_exp &y, const double_exp &x);

    static void sin(double_exp *result, const double_exp &v);

    static double_exp sin(const double_exp &v);

    static void cos(double_exp *result, const double_exp &v);

    static double_exp cos(const double_exp &v);

    static void tan(double_exp *result, const double_exp &v);

    static double_exp tan(const double_exp &v);

    static void hypot_approx(double_exp *result, const double_exp &a, const double_exp &b);

    static double_exp hypot_approx(const double_exp &a, const double_exp &b);

    static void hypot(double_exp *result, const double_exp &a, const double_exp &b);

    static double_exp hypot(const double_exp &a, const double_exp &b);

    static void hypot2(double_exp *result, const double_exp &a, const double_exp &b);

    static double_exp hypot2(const double_exp &a, const double_exp &b);
};


inline void dex_trigonometric::atan2(double_exp *result, const double_exp &y, const double_exp &x) {
    const double dv = std::atan2(static_cast<double>(y), static_cast<double>(x));
    double_exp::dex_cpy(result, y);
    if (dv == 0) {
        return;
    }
    *result /= x;
}

inline double_exp dex_trigonometric::atan2(const double_exp &y, const double_exp &x) {
    double_exp result = RFF::Precision::DEX_ZERO;
    atan2(&result, y, x);
    return result;
}


inline void dex_trigonometric::sin(double_exp *result, const double_exp &v) {
    const double dv = std::sin(static_cast<double>(v));
    if (dv == 0) {
        double_exp::dex_cpy(result, v);
    }
    double_exp::dex_cpy(result, dv);
}


inline double_exp dex_trigonometric::sin(const double_exp &v) {
    double_exp result = RFF::Precision::DEX_ZERO;
    sin(&result, v);
    return result;
}

inline void dex_trigonometric::cos(double_exp *result, const double_exp &v) {
    const double dv = std::cos(static_cast<double>(v));
    double_exp::dex_cpy(result, dv);
}

inline double_exp dex_trigonometric::cos(const double_exp &v) {
    double_exp result = RFF::Precision::DEX_ZERO;
    cos(&result, v);
    return result;
}

inline void dex_trigonometric::tan(double_exp *result, const double_exp &v) {
    const double dv = std::tan(static_cast<double>(v));
    if (dv == 0) {
        double_exp::dex_cpy(result, v);
    }
    double_exp::dex_cpy(result, dv);
}

inline double_exp dex_trigonometric::tan(const double_exp &v) {
    double_exp result = RFF::Precision::DEX_ZERO;
    tan(&result, v);
    return result;
}

inline void dex_trigonometric::hypot_approx(double_exp *result, const double_exp &a, const double_exp &b) {
    const double_exp a_abs = dex_std::abs(a);
    const double_exp b_abs = dex_std::abs(b);
    const double_exp mn = dex_std::min(a_abs, b_abs);
    const double_exp mx = dex_std::max(a_abs, b_abs);


    if (mn.is_zero()) {
        double_exp::dex_cpy(result, mx);
        return;
    }

    if (mx.is_zero()) {
        double_exp::dex_cpy(result, RFF::Precision::DEX_ZERO);
        return;
    }
    double_exp::dex_sqr(result, mn);
    *result *= 0.428;
    double_exp::dex_div(result, *result, mx);
    double_exp::dex_add(result, *result, mx);
}

inline double_exp dex_trigonometric::hypot_approx(const double_exp &a, const double_exp &b) {
    double_exp result = RFF::Precision::DEX_ZERO;
    hypot_approx(&result, a, b);
    return result;
}

inline void dex_trigonometric::hypot(double_exp *result, const double_exp &a, const double_exp &b) {
    hypot2(result, a, b);
    double_exp::dex_sqrt(result, *result);
}

inline double_exp dex_trigonometric::hypot(const double_exp &a, const double_exp &b) {
    double_exp result = RFF::Precision::DEX_ZERO;
    hypot(&result, a, b);
    return result;
}

inline void dex_trigonometric::hypot2(double_exp *result, const double_exp &a, const double_exp &b) {
    double_exp sqr_temp = RFF::Precision::DEX_ZERO;
    double_exp::dex_sqr(&sqr_temp, a);
    double_exp::dex_sqr(result, b);
    double_exp::dex_add(result, *result, sqr_temp);
}

inline double_exp dex_trigonometric::hypot2(const double_exp &a, const double_exp &b) {
    double_exp result = RFF::Precision::DEX_ZERO;
    hypot2(&result, a, b);
    return result;
}