//
// Created by Merutilm on 2025-05-17.
//

#pragma once
#include <cmath>
#include <string>

#include "../ui/RFF.h"

struct dex_exp;
struct dex_std;
struct dex_trigonometric;

class double_exp {

    int exp2;
    double mantissa;

    friend dex_exp;
    friend dex_std;
    friend dex_trigonometric;

public:

    static const double_exp DEX_ZERO;
    static const double_exp DEX_ONE;
    static const double_exp DEX_NAN;
    static const double_exp DEX_POSITIVE_INFINITY;
    static const double_exp DEX_NEGATIVE_INFINITY;
    static constexpr double NORMALIZE_CONSTANT_MAX = 1e150;
    static constexpr double NORMALIZE_CONSTANT_MIN = 1e-150;

    explicit double_exp();

    explicit double_exp(int exp2, double mantissa);

    static void dex_add(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_sub(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_mul(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_div(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_sqr(double_exp *result, const double_exp &v);

    static void dex_sqrt(double_exp *result, const double_exp &v);

    static void dex_mul_2exp(double_exp *result, const double_exp &v, int exp2);

    static void dex_div_2exp(double_exp *result, const double_exp &v, int exp2);

    static void dex_neg(double_exp *result);

    static void dex_cpy(double_exp *result, double v);

    static void dex_cpy(double_exp *result, int exp2, double mantissa);

    static void dex_cpy(double_exp *result, const double_exp &v);

    bool operator==(const double_exp &other) const = default;

    friend double_exp operator+(const double_exp &a, const double_exp &b);

    friend double_exp operator+(const double_exp &a, double b);

    friend double_exp operator+(double a, const double_exp &b);

    friend double_exp operator-(const double_exp &a, const double_exp &b);

    friend double_exp operator-(const double_exp &a);

    friend double_exp operator-(const double_exp &a, double b);

    friend double_exp operator-(double a, const double_exp &b);

    friend double_exp operator*(const double_exp &a, const double_exp &b);

    friend double_exp operator*(const double_exp &a, double b);

    friend double_exp operator*(double a, const double_exp &b);

    friend double_exp operator/(const double_exp &a, const double_exp &b);

    friend double_exp operator/(const double_exp &a, double b);

    friend double_exp operator/(double a, const double_exp &b);

    double_exp &operator+=(const double_exp &other);

    double_exp &operator+=(double other);

    double_exp &operator-=(const double_exp &other);

    double_exp &operator-=(double other);

    double_exp &operator*=(const double_exp &other);

    double_exp &operator*=(double other);

    double_exp &operator/=(const double_exp &other);

    double_exp &operator/=(double other);

    std::partial_ordering operator<=>(const double_exp &other) const;

    std::partial_ordering operator<=>(double other) const;

    static double_exp value(double value);

    static void normalize(double_exp *result);

    char sgn() const;

    bool isinf() const;

    bool isnan() const;

    bool is_zero() const;

    explicit operator double() const;

    std::string to_string() const;

    int get_exp2() const;

    double get_mantissa() const;

    bool normalize_required() const;
};

// DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP
// DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP
// DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP
// DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP
// DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP  DEFINITION OF DOUBLE EXP
// Move to header for 20-30% performance improvement


inline const double_exp double_exp::DEX_ZERO = double_exp(0, 0);
inline const double_exp double_exp::DEX_ONE = double_exp(0, 1);
inline const double_exp double_exp::DEX_NAN = double_exp(0, NAN);
inline const double_exp double_exp::DEX_POSITIVE_INFINITY = double_exp(INT_MAX, INFINITY);
inline const double_exp double_exp::DEX_NEGATIVE_INFINITY = double_exp(INT_MAX, -INFINITY);

inline double_exp::double_exp() : double_exp(0, 0) {
}


inline double_exp::double_exp(const int exp2, const double mantissa) : exp2(exp2), mantissa(mantissa) {
}


inline void double_exp::dex_add(double_exp *result, const double_exp &a, const double_exp &b) {
    const int d_exp2 = a.exp2 - b.exp2;
    const double mantissa = ldexp(a.mantissa, std::min(0, d_exp2)) + ldexp(b.mantissa, std::min(0, -d_exp2));
    result->exp2 = std::max(a.exp2, b.exp2);
    result->mantissa = mantissa;
}

inline void double_exp::dex_sub(double_exp *result, const double_exp &a, const double_exp &b) {
    const int d_exp2 = a.exp2 - b.exp2;
    const double mantissa = ldexp(a.mantissa, std::min(0, d_exp2)) - ldexp(b.mantissa, std::min(0, -d_exp2));
    result->exp2 = std::max(a.exp2, b.exp2);
    result->mantissa = mantissa;
    normalize(result);
}

inline void double_exp::dex_mul(double_exp *result, const double_exp &a, const double_exp &b) {
    result->exp2 = a.exp2 + b.exp2;
    result->mantissa = a.mantissa * b.mantissa;
}

inline void double_exp::dex_div(double_exp *result, const double_exp &a, const double_exp &b) {
    result->exp2 = a.exp2 - b.exp2;
    result->mantissa = a.mantissa / b.mantissa;
}

inline void double_exp::dex_sqr(double_exp *result, const double_exp &a) {
    result->exp2 = a.exp2 << 1;
    result->mantissa = a.mantissa * a.mantissa;
}


inline void double_exp::dex_sqrt(double_exp *result, const double_exp &a) {
    result->exp2 = a.exp2 >> 1;
    result->mantissa = a.sgn() * sqrt(std::abs(a.mantissa));
}

inline void double_exp::dex_mul_2exp(double_exp *result, const double_exp &v, const int exp2) {
    result->exp2 = v.exp2 + exp2;
    result->mantissa = v.mantissa;
}

inline void double_exp::dex_div_2exp(double_exp *result, const double_exp &v, const int exp2) {
    result->exp2 = v.exp2 - exp2;
    result->mantissa = v.mantissa;
}
inline void double_exp::dex_neg(double_exp *result) {
    result->mantissa = -result->mantissa;
}

inline void double_exp::dex_cpy(double_exp *result, const double v) {
    result->exp2 = 0;
    result->mantissa = v;
    normalize(result);
}

inline void double_exp::dex_cpy(double_exp *result, const int exp2, const double mantissa) {
    result->exp2 = exp2;
    result->mantissa = mantissa;
}


inline void double_exp::dex_cpy(double_exp *result, const double_exp &v) {
    result->exp2 = v.exp2;
    result->mantissa = v.mantissa;
}


inline double_exp operator+(const double_exp &a, const double_exp &b) {
    if (a.isnan() || b.isnan()) {
        return double_exp::DEX_NAN;
    }
    if (a.isinf() && b.isinf()) {
        if (a.sgn() == b.sgn()) {
            return a;
        }
        return double_exp::DEX_NAN;
    }
    if (a.isinf() || b.is_zero()) {
        return a;
    }
    if (b.isinf() || a.is_zero()) {
        return b;
    }
    auto result = double_exp(0, 0);
    double_exp::dex_add(&result, a, b);
    double_exp::normalize(&result);
    return result;
}

inline double_exp operator+(const double_exp &a, const double b) {
    return a + double_exp::value(b);
}

inline double_exp operator+(const double a, const double_exp &b) {
    return double_exp::value(a) + b;
}

inline double_exp operator-(const double_exp &a, const double_exp &b) {
    if (a.isnan() || b.isnan()) {
        return double_exp::DEX_NAN;
    }
    if (a.isinf() && b.isinf()) {
        if (a.sgn() == b.sgn()) {
            return double_exp::DEX_NAN;
        }
        return a;
    }
    if (a.isinf() || b.is_zero()) {
        return a;
    }
    if (b.isinf() || a.is_zero()) {
        return -b;
    }
    auto result = double_exp(0, 0);
    double_exp::dex_sub(&result, a, b);
    double_exp::normalize(&result);
    return result;
}

inline double_exp operator-(const double_exp &a) {
    return double_exp(a.exp2, -a.mantissa);
}

inline double_exp operator-(const double_exp &a, const double b) {
    return a - double_exp::value(b);
}

inline double_exp operator-(const double a, const double_exp &b) {
    return double_exp::value(a) - b;
}

inline double_exp operator*(const double_exp &a, const double_exp &b) {
    if (a.is_zero() || b.is_zero()) {
        return double_exp::DEX_ZERO;
    }
    if (a.isnan() || b.isnan()) {
        return double_exp::DEX_NAN;
    }
    if (a.isinf() || b.isinf()) {
        return a.sgn() == b.sgn() ? double_exp::DEX_POSITIVE_INFINITY : double_exp::DEX_NEGATIVE_INFINITY;
    }
    auto result = double_exp(0, 0);
    double_exp::dex_mul(&result, a, b);
    double_exp::normalize(&result);
    return result;
}


inline double_exp operator*(const double_exp &a, const double b) {
    return a * double_exp::value(b);
}

inline double_exp operator*(const double a, const double_exp &b) {
    return double_exp::value(a) * b;
}

inline double_exp operator/(const double_exp &a, const double_exp &b) {
    if (a.is_zero() || b.isinf()) {
        return double_exp::DEX_ZERO;
    }
    if (b.is_zero() || a.isinf()) {
        return a.sgn() == 1 ? double_exp::DEX_POSITIVE_INFINITY : double_exp::DEX_NEGATIVE_INFINITY;
    }
    if (a.is_zero() && b.is_zero()) {
        return double_exp::DEX_NAN;
    }
    auto result = double_exp(0, 0);
    double_exp::dex_div(&result, a, b);
    double_exp::normalize(&result);
    return result;
}

inline double_exp operator/(const double_exp &a, const double b) {
    return a / double_exp::value(b);
}

inline double_exp operator/(const double a, const double_exp &b) {
    return double_exp::value(a) / b;
}

inline double_exp &double_exp::operator+=(const double_exp &other) {
    dex_add(this, *this, other);
    normalize(this);
    return *this;
}

inline double_exp &double_exp::operator+=(const double other) {
    return *this += value(other);
}

inline double_exp &double_exp::operator-=(const double_exp &other) {
    dex_sub(this, *this, other);
    normalize(this);
    return *this;
}

inline double_exp &double_exp::operator-=(const double other) {
    return *this -= value(other);
}

inline double_exp &double_exp::operator*=(const double_exp &other) {
    dex_mul(this, *this, other);
    normalize(this);
    return *this;
}

inline double_exp &double_exp::operator*=(const double other) {
    return *this *= value(other);
}

inline double_exp &double_exp::operator/=(const double_exp &other) {
    dex_div(this, *this, other);
    normalize(this);
    return *this;
}

inline double_exp &double_exp::operator/=(const double other) {
    return *this /= value(other);
}

inline std::partial_ordering double_exp::operator<=>(const double_exp &other) const {
    const double_exp v = *this - other;
    if (v.isnan()) {
        return std::partial_ordering::unordered;
    }
    return v.sgn() <=> 0;
}

inline std::partial_ordering double_exp::operator<=>(const double other) const {
    return *this <=> value(other);
}

inline double_exp double_exp::value(const double value) {
    if (value == 0) {
        return DEX_ZERO;
    }
    if (std::isinf(value)) {
        return value > 0 ? DEX_POSITIVE_INFINITY : DEX_NEGATIVE_INFINITY;
    }
    if (std::isnan(value)) {
        return DEX_NAN;
    }

    auto result = double_exp(0, 0);
    dex_cpy(&result, value);
    return result;
}

inline void double_exp::normalize(double_exp *result) {
    int dex;
    result->mantissa = std::frexp(result->mantissa, &dex);
    result->exp2 += dex;
}

inline char double_exp::sgn() const {
    return static_cast<char>(0 < mantissa) - static_cast<char>(mantissa < 0);
}

inline bool double_exp::isinf() const {
    return DEX_POSITIVE_INFINITY == *this || DEX_NEGATIVE_INFINITY == *this;
}

inline bool double_exp::isnan() const {
    return std::isnan(mantissa);
}

inline bool double_exp::is_zero() const {
    return sgn() == 0;
}


inline double_exp::operator double() const {
    return ldexp(mantissa, exp2);
}


inline std::string double_exp::to_string() const {
    // m * 2^n
    // = m * 10^(log10(2) * n)
    // = exp10 = log10(2) * n
    if (isnan()) {
        return "nan";
    }
    if (isinf()) {
        return sgn() > 0 ? "inf" : "-inf";
    }
    const double raw_exp10 = RFF::Precision::LOG10_2 * exp2;
    auto exp10 = static_cast<int>(raw_exp10);
    double mantissa10 = mantissa * std::pow(10, raw_exp10 - exp10);

    while (std::abs(mantissa10) < 1 && mantissa10 != 0) {
        mantissa10 *= 10;
        exp10 -= 1;
    }
    return std::format("{}e{}", mantissa10, exp10);
}


inline int double_exp::get_exp2() const {
    return exp2;
}

inline double double_exp::get_mantissa() const {
    return mantissa;
}

inline bool double_exp::normalize_required() const {
    return mantissa * sgn() > NORMALIZE_CONSTANT_MAX || mantissa * sgn() < NORMALIZE_CONSTANT_MIN;
}
