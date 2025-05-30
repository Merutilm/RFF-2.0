//
// Created by Merutilm on 2025-05-17.
//

#pragma once
#include <cmath>
#include <format>
#include <string>
#include <stdint.h>

#include "../ui/RFF.h"


struct dex_exp;
struct dex_std;
struct dex_trigonometric;

/**
 *
 * 
 */
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
    static const double_exp DEX_POS_INF;
    static const double_exp DEX_NEG_INF;
    static constexpr double NORMALIZE_CONSTANT_MAX = 1e75;
    static constexpr double NORMALIZE_CONSTANT_MIN = 1e-75;

    explicit double_exp();

    explicit double_exp(int exp2, double mantissa);

    static void dex_add(double_exp *result, const double_exp &a, const double_exp &b);

    static void dex_sub(double_exp *result, const double_exp &a, const double_exp &b);

    static double dex_ldexp_neg(double mantissa, int exp2);

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

    friend double_exp operator+(const double_exp &a, const double_exp &b) {
        if (a.isnan() || b.isnan()) {
            return DEX_NAN;
        }
        if (a.isinf() && b.isinf()) {
            if (a.sgn() == b.sgn()) {
                return a;
            }
            return DEX_NAN;
        }
        if (a.isinf() || b.is_zero()) {
            return a;
        }
        if (b.isinf() || a.is_zero()) {
            return b;
        }
        auto result = double_exp(0, 0);
        dex_add(&result, a, b);
        normalize(&result);
        return result;
    }

    friend double_exp operator+(const double_exp &a, const double b) {
        return a + value(b);
    }

    friend double_exp operator+(const double a, const double_exp &b) {
        return value(a) + b;
    }

    friend double_exp operator-(const double_exp &a, const double_exp &b) {
        if (a.isnan() || b.isnan()) {
            return DEX_NAN;
        }
        if (a.isinf() && b.isinf()) {
            if (a.sgn() == b.sgn()) {
                return DEX_NAN;
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
        dex_sub(&result, a, b);
        normalize(&result);
        return result;
    }

    friend double_exp operator-(const double_exp &a) {
        return double_exp(a.exp2, -a.mantissa);
    }

    friend double_exp operator-(const double_exp &a, const double b) {
        return a - value(b);
    }

    friend double_exp operator-(const double a, const double_exp &b) {
        return value(a) - b;
    }

    friend double_exp operator*(const double_exp &a, const double_exp &b) {
        if (a.is_zero() || b.is_zero()) {
            return DEX_ZERO;
        }
        if (a.isnan() || b.isnan()) {
            return DEX_NAN;
        }
        if (a.isinf() || b.isinf()) {
            return a.sgn() == b.sgn() ? DEX_POS_INF : DEX_NEG_INF;
        }
        auto result = double_exp(0, 0);
        dex_mul(&result, a, b);
        normalize(&result);
        return result;
    }

    friend double_exp operator*(const double_exp &a, const double b) {
        return a * value(b);
    }

    friend double_exp operator*(const double a, const double_exp &b) {
        return value(a) * b;
    }

    friend double_exp operator/(const double_exp &a, const double_exp &b) {
        if (a.is_zero() && b.is_zero()) {
            return DEX_NAN;
        }
        if (a.is_zero() || b.isinf()) {
            return DEX_ZERO;
        }
        if (b.is_zero() || a.isinf()) {
            return a.sgn() == b.sgn() ? DEX_POS_INF : DEX_NEG_INF;
        }

        auto result = double_exp(0, 0);
        dex_div(&result, a, b);
        normalize(&result);
        return result;
    }

    friend double_exp operator/(const double_exp &a, const double b) {
        return a / value(b);
    }

    friend double_exp operator/(const double a, const double_exp &b) {
        return value(a) / b;
    }

    friend double_exp &operator+=(double_exp &a, const double_exp &b) {
        if (a.isnan() || b.isnan()) {
            dex_cpy(&a, DEX_NAN);
            return a;
        }
        if (a.isinf() && b.isinf()) {
            if (a.sgn() == b.sgn()) {
                return a;
            }
            dex_cpy(&a, DEX_NAN);
            return a;
        }
        if (a.isinf() || b.is_zero()) {
            return a;
        }
        if (b.isinf() || a.is_zero()) {
            dex_cpy(&a, b);
            return a;
        }
        dex_add(&a, a, b);
        normalize(&a);
        return a;
    }

    friend double_exp &operator+=(double_exp &a, const double b) {
        return a += value(b);
    }

    friend double_exp &operator-=(double_exp &a, const double_exp &b) {
        if (a.isnan() || b.isnan()) {
            dex_cpy(&a, DEX_NAN);
            return a;
        }
        if (a.isinf() && b.isinf()) {
            if (a.sgn() != b.sgn()) {
                return a;
            }
            dex_cpy(&a, DEX_NAN);
            return a;
        }
        if (a.isinf() || b.is_zero()) {
            return a;
        }
        if (b.isinf() || a.is_zero()) {
            dex_cpy(&a, b);
            dex_neg(&a);
            return a;
        }
        dex_sub(&a, a, b);
        normalize(&a);
        return a;
    }

    friend double_exp &operator-=(double_exp &a, const double b) {
        return a -= value(b);
    }

    friend double_exp &operator*=(double_exp &a, const double_exp &b) {
        if (a.is_zero() || b.is_zero()) {
            dex_cpy(&a, DEX_ZERO);
            return a;
        }
        if (a.isnan() || b.isnan()) {
            dex_cpy(&a, DEX_NAN);
            return a;
        }
        if (a.isinf() || b.isinf()) {
            if (a.sgn() == b.sgn()) dex_cpy(&a, DEX_POS_INF);
            else dex_cpy(&a, DEX_NEG_INF);
            return a;
        }
        dex_mul(&a, a, b);
        normalize(&a);
        return a;
    }

    friend double_exp &operator*=(double_exp &a, const double b) {
        return a *= value(b);
    }

    friend double_exp &operator/=(double_exp &a, const double_exp &b) {
        if (a.is_zero() && b.is_zero()) {
            dex_cpy(&a, DEX_NAN);
            return a;
        }
        if (a.is_zero() || b.isinf()) {
            dex_cpy(&a, DEX_ZERO);
            return a;
        }
        if (b.is_zero() || a.isinf()) {
            if (a.sgn() == b.sgn()) dex_cpy(&a, DEX_POS_INF);
            else dex_cpy(&a, DEX_NEG_INF);
            return a;
        }

        dex_div(&a, a, b);
        normalize(&a);
        return a;
    }

    friend double_exp &operator/=(double_exp &a, const double b) {
        return a /= value(b);
    }

    friend std::partial_ordering operator<=>(const double_exp &a, const double_exp &b) {
        const double_exp v = a - b;
        if (v.isnan()) {
            return std::partial_ordering::unordered;
        }
        return v.sgn() <=> 0;
    }

    friend std::partial_ordering operator<=>(const double_exp &a, const double b) {
        return a <=> value(b);
    }

    friend std::partial_ordering operator<=>(double_exp &a, const double_exp &b);

    friend std::partial_ordering operator<=>(double_exp &a, double b);

    static double_exp value(double value);

    static void normalize(double_exp *target);

    char sgn() const;

    bool isinf() const;

    bool isnan() const;

    bool is_zero() const;

    explicit operator double() const;

    std::string to_string() const;

    int get_exp2() const;

    double get_mantissa() const;

    void try_normalize();
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

inline const double_exp double_exp::DEX_POS_INF = double_exp(0, INFINITY);

inline const double_exp double_exp::DEX_NEG_INF = double_exp(0, -INFINITY);

inline double_exp::double_exp() : double_exp(0, 0) {
}

inline double_exp::double_exp(const int exp2, const double mantissa) : exp2(exp2), mantissa(mantissa) {
}

inline void double_exp::dex_add(double_exp *result, const double_exp &a, const double_exp &b) {
    const int d_exp2 = a.exp2 - b.exp2;
    result->exp2 = std::max(a.exp2, b.exp2);
    result->mantissa = dex_ldexp_neg(a.mantissa, std::min(0, d_exp2)) + dex_ldexp_neg(b.mantissa, std::min(0, -d_exp2));
}

inline void double_exp::dex_sub(double_exp *result, const double_exp &a, const double_exp &b) {
    const int d_exp2 = a.exp2 - b.exp2;
    result->exp2 = std::max(a.exp2, b.exp2);
    result->mantissa = dex_ldexp_neg(a.mantissa, std::min(0, d_exp2)) - dex_ldexp_neg(b.mantissa, std::min(0, -d_exp2));
}

inline double double_exp::dex_ldexp_neg(const double mantissa, const int exp2) {
    const auto mts_bits = std::bit_cast<uint64_t>(mantissa);
    const auto mts_ubits = mts_bits & 0x7fffffffffffffffULL;
    if (const auto f_shift = static_cast<int>(mts_ubits >> 52) + exp2; f_shift < 0) {
        return 0; //Do not consider ~e-309.
    }
    return std::bit_cast<double>(mts_bits - (static_cast<uint64_t>(-exp2) << 52));
}

inline void double_exp::dex_mul(double_exp *result, const double_exp &a, const double_exp &b) {
    result->exp2 = a.exp2 + b.exp2;
    result->mantissa = a.mantissa * b.mantissa;
}

inline void double_exp::dex_div(double_exp *result, const double_exp &a, const double_exp &b) {
    result->exp2 = a.exp2 - b.exp2;
    result->mantissa = a.mantissa / b.mantissa;
}

inline void double_exp::dex_sqr(double_exp *result, const double_exp &v) {
    result->exp2 = v.exp2 << 1;
    result->mantissa = v.mantissa * v.mantissa;
}

inline void double_exp::dex_sqrt(double_exp *result, const double_exp &v) {
    result->exp2 = v.exp2 >> 1;
    result->mantissa = v.sgn() * sqrt(std::abs(v.mantissa));
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

inline void double_exp::normalize(double_exp *target) {
    if (target->mantissa == 0) {
        dex_cpy(target, DEX_ZERO);
        return;
    }
    if (target->isinf()) {
        dex_cpy(target, target->sgn() ? DEX_POS_INF : DEX_NEG_INF);
        return;
    }
    if (target->isnan()) {
        dex_cpy(target, DEX_NAN);
        return;
    }

    const auto mts_bits = std::bit_cast<uint64_t>(target->mantissa);
    target->mantissa = std::bit_cast<double>(mts_bits & 0x800fffffffffffffULL | 0x3fe0000000000000ULL);
    target->exp2 += static_cast<int>((mts_bits & 0x7ff0000000000000ULL) >> 52) - 0x03fe;
}

inline char double_exp::sgn() const {
    return static_cast<char>(0 < mantissa) - static_cast<char>(mantissa < 0);
}

inline bool double_exp::isinf() const {
    return std::isinf(mantissa);
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
    const double raw_exp10 = RFF::Constant::LOG10_2 * exp2;
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

inline void double_exp::try_normalize() {
    if (mantissa * sgn() > NORMALIZE_CONSTANT_MAX || mantissa * sgn() < NORMALIZE_CONSTANT_MIN) {
        normalize(this);
    }
}

inline double_exp double_exp::value(const double value) {
    if (value == 0) {
        return DEX_ZERO;
    }
    if (std::isinf(value)) {
        return value > 0 ? DEX_POS_INF : DEX_NEG_INF;
    }
    if (std::isnan(value)) {
        return DEX_NAN;
    }

    auto result = double_exp(0, 0);
    dex_cpy(&result, value);
    return result;
}
