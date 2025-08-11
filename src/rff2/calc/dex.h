//
// Created by Merutilm on 2025-05-17.
//

#pragma once
#include <cmath>
#include <format>
#include <string>
#include <stdint.h>

#include "../constants/Constants.hpp"


namespace merutilm::rff2 {


    struct dex_exp;
    struct dex_std;
    struct dex_trigonometric;

    /**
     * the floating-point object which supports semi-infinity exponents.
     */
    class dex {
        int exp2;
        double mantissa;

        friend dex_exp;
        friend dex_std;
        friend dex_trigonometric;

    public:
        static const dex ZERO;
        static const dex ONE;
        static const dex NN;
        static const dex PINF;
        static const dex NINF;
        static constexpr double NORMALIZE_CONSTANT_MAX = 1e75;
        static constexpr double NORMALIZE_CONSTANT_MIN = 1e-75;

        explicit dex();

        explicit dex(int exp2, double mantissa);

        static void add(dex *result, const dex &a, const dex &b);

        static void sub(dex *result, const dex &a, const dex &b);

        static double ldexp_neg(double mantissa, int exp2);

        static void mul(dex *result, const dex &a, const dex &b);

        static void div(dex *result, const dex &a, const dex &b);

        static void sqr(dex *result, const dex &v);

        static void sqrt(dex *result, const dex &v);

        static void mul_2exp(dex *result, const dex &v, int exp2);

        static void div_2exp(dex *result, const dex &v, int exp2);

        static void neg(dex *result);

        static void cpy(dex *result, double v);

        static void cpy(dex *result, int exp2, double mantissa);

        static void cpy(dex *result, const dex &v);

        bool operator==(const dex &other) const = default;

        friend dex operator+(const dex &a, const dex &b) {
            if (a.isnan() || b.isnan()) {
                return NN;
            }
            if (a.isinf() && b.isinf()) {
                if (a.sgn() == b.sgn()) {
                    return a;
                }
                return NN;
            }
            if (a.isinf() || b.is_zero()) {
                return a;
            }
            if (b.isinf() || a.is_zero()) {
                return b;
            }
            auto result = dex(0, 0);
            add(&result, a, b);
            normalize(&result);
            return result;
        }

        friend dex operator+(const dex &a, const double b) {
            return a + value(b);
        }

        friend dex operator+(const double a, const dex &b) {
            return value(a) + b;
        }

        friend dex operator-(const dex &a, const dex &b) {
            if (a.isnan() || b.isnan()) {
                return NN;
            }
            if (a.isinf() && b.isinf()) {
                if (a.sgn() == b.sgn()) {
                    return NN;
                }
                return a;
            }
            if (a.isinf() || b.is_zero()) {
                return a;
            }
            if (b.isinf() || a.is_zero()) {
                return -b;
            }
            auto result = dex(0, 0);
            sub(&result, a, b);
            normalize(&result);
            return result;
        }

        friend dex operator-(const dex &a) {
            return dex(a.exp2, -a.mantissa);
        }

        friend dex operator-(const dex &a, const double b) {
            return a - value(b);
        }

        friend dex operator-(const double a, const dex &b) {
            return value(a) - b;
        }

        friend dex operator*(const dex &a, const dex &b) {
            if (a.is_zero() || b.is_zero()) {
                return ZERO;
            }
            if (a.isnan() || b.isnan()) {
                return NN;
            }
            if (a.isinf() || b.isinf()) {
                return a.sgn() == b.sgn() ? PINF : NINF;
            }
            auto result = dex(0, 0);
            mul(&result, a, b);
            normalize(&result);
            return result;
        }

        friend dex operator*(const dex &a, const double b) {
            return a * value(b);
        }

        friend dex operator*(const double a, const dex &b) {
            return value(a) * b;
        }

        friend dex operator/(const dex &a, const dex &b) {
            if (a.is_zero() && b.is_zero()) {
                return NN;
            }
            if (a.is_zero() || b.isinf()) {
                return ZERO;
            }
            if (b.is_zero() || a.isinf()) {
                return a.sgn() == b.sgn() ? PINF : NINF;
            }

            auto result = dex(0, 0);
            div(&result, a, b);
            normalize(&result);
            return result;
        }

        friend dex operator/(const dex &a, const double b) {
            return a / value(b);
        }

        friend dex operator/(const double a, const dex &b) {
            return value(a) / b;
        }

        friend dex &operator+=(dex &a, const dex &b) {
            if (a.isnan() || b.isnan()) {
                cpy(&a, NN);
                return a;
            }
            if (a.isinf() && b.isinf()) {
                if (a.sgn() == b.sgn()) {
                    return a;
                }
                cpy(&a, NN);
                return a;
            }
            if (a.isinf() || b.is_zero()) {
                return a;
            }
            if (b.isinf() || a.is_zero()) {
                cpy(&a, b);
                return a;
            }
            add(&a, a, b);
            normalize(&a);
            return a;
        }

        friend dex &operator+=(dex &a, const double b) {
            return a += value(b);
        }

        friend dex &operator-=(dex &a, const dex &b) {
            if (a.isnan() || b.isnan()) {
                cpy(&a, NN);
                return a;
            }
            if (a.isinf() && b.isinf()) {
                if (a.sgn() != b.sgn()) {
                    return a;
                }
                cpy(&a, NN);
                return a;
            }
            if (a.isinf() || b.is_zero()) {
                return a;
            }
            if (b.isinf() || a.is_zero()) {
                cpy(&a, b);
                neg(&a);
                return a;
            }
            sub(&a, a, b);
            normalize(&a);
            return a;
        }

        friend dex &operator-=(dex &a, const double b) {
            return a -= value(b);
        }

        friend dex &operator*=(dex &a, const dex &b) {
            if (a.is_zero() || b.is_zero()) {
                cpy(&a, ZERO);
                return a;
            }
            if (a.isnan() || b.isnan()) {
                cpy(&a, NN);
                return a;
            }
            if (a.isinf() || b.isinf()) {
                if (a.sgn() == b.sgn()) cpy(&a, PINF);
                else cpy(&a, NINF);
                return a;
            }
            mul(&a, a, b);
            normalize(&a);
            return a;
        }

        friend dex &operator*=(dex &a, const double b) {
            return a *= value(b);
        }

        friend dex &operator/=(dex &a, const dex &b) {
            if (a.is_zero() && b.is_zero()) {
                cpy(&a, NN);
                return a;
            }
            if (a.is_zero() || b.isinf()) {
                cpy(&a, ZERO);
                return a;
            }
            if (b.is_zero() || a.isinf()) {
                if (a.sgn() == b.sgn()) cpy(&a, PINF);
                else cpy(&a, NINF);
                return a;
            }

            div(&a, a, b);
            normalize(&a);
            return a;
        }

        friend dex &operator/=(dex &a, const double b) {
            return a /= value(b);
        }

        friend std::partial_ordering operator<=>(const dex &a, const dex &b) {
            const dex v = a - b;
            if (v.isnan()) {
                return std::partial_ordering::unordered;
            }
            return v.sgn() <=> 0;
        }

        friend std::partial_ordering operator<=>(const dex &a, const double b) {
            return a <=> value(b);
        }

        friend std::partial_ordering operator<=>(dex &a, const dex &b);

        friend std::partial_ordering operator<=>(dex &a, double b);

        static dex value(double value);

        static void normalize(dex *target);

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


    inline const dex dex::ZERO = dex(0, 0);

    inline const dex dex::ONE = dex(0, 1);

    inline const dex dex::NN = dex(0, NAN);

    inline const dex dex::PINF = dex(0, INFINITY);

    inline const dex dex::NINF = dex(0, -INFINITY);

    inline dex::dex() : dex(0, 0) {
    }

    inline dex::dex(const int exp2, const double mantissa) : exp2(exp2), mantissa(mantissa) {
    }

    inline void dex::add(dex *result, const dex &a, const dex &b) {
        const int d_exp2 = a.exp2 - b.exp2;
        result->exp2 = std::max(a.exp2, b.exp2);
        result->mantissa = ldexp_neg(a.mantissa, std::min(0, d_exp2)) + ldexp_neg(b.mantissa, std::min(0, -d_exp2));
    }

    inline void dex::sub(dex *result, const dex &a, const dex &b) {
        const int d_exp2 = a.exp2 - b.exp2;
        result->exp2 = std::max(a.exp2, b.exp2);
        result->mantissa = ldexp_neg(a.mantissa, std::min(0, d_exp2)) - ldexp_neg(b.mantissa, std::min(0, -d_exp2));
    }

    inline double dex::ldexp_neg(const double mantissa, const int exp2) {
        const auto mts_bits = std::bit_cast<uint64_t>(mantissa);
        const auto mts_ubits = mts_bits & 0x7fffffffffffffffULL;
        if (const auto f_shift = static_cast<int>(mts_ubits >> 52) + exp2; f_shift < 0) {
            return 0; //Do not consider ~e-309.
        }
        return std::bit_cast<double>(mts_bits - (static_cast<uint64_t>(-exp2) << 52));
    }

    inline void dex::mul(dex *result, const dex &a, const dex &b) {
        result->exp2 = a.exp2 + b.exp2;
        result->mantissa = a.mantissa * b.mantissa;
    }

    inline void dex::div(dex *result, const dex &a, const dex &b) {
        result->exp2 = a.exp2 - b.exp2;
        result->mantissa = a.mantissa / b.mantissa;
    }

    inline void dex::sqr(dex *result, const dex &v) {
        result->exp2 = v.exp2 << 1;
        result->mantissa = v.mantissa * v.mantissa;
    }

    inline void dex::sqrt(dex *result, const dex &v) {
        result->exp2 = v.exp2 >> 1;
        result->mantissa = v.sgn() * std::sqrt(std::abs(v.mantissa));
    }

    inline void dex::mul_2exp(dex *result, const dex &v, const int exp2) {
        result->exp2 = v.exp2 + exp2;
        result->mantissa = v.mantissa;
    }

    inline void dex::div_2exp(dex *result, const dex &v, const int exp2) {
        result->exp2 = v.exp2 - exp2;
        result->mantissa = v.mantissa;
    }

    inline void dex::neg(dex *result) {
        result->mantissa = -result->mantissa;
    }

    inline void dex::cpy(dex *result, const double v) {
        result->exp2 = 0;
        result->mantissa = v;
        normalize(result);
    }

    inline void dex::cpy(dex *result, const int exp2, const double mantissa) {
        result->exp2 = exp2;
        result->mantissa = mantissa;
    }

    inline void dex::cpy(dex *result, const dex &v) {
        result->exp2 = v.exp2;
        result->mantissa = v.mantissa;
    }

    inline void dex::normalize(dex *target) {
        if (target->mantissa == 0) {
            cpy(target, ZERO);
            return;
        }
        if (target->isinf()) {
            cpy(target, target->sgn() ? PINF : NINF);
            return;
        }
        if (target->isnan()) {
            cpy(target, NN);
            return;
        }

        const auto mts_bits = std::bit_cast<uint64_t>(target->mantissa);
        target->mantissa = std::bit_cast<double>(mts_bits & 0x800fffffffffffffULL | 0x3fe0000000000000ULL);
        target->exp2 += static_cast<int>((mts_bits & 0x7ff0000000000000ULL) >> 52) - 0x03fe;
    }

    inline char dex::sgn() const {
        return static_cast<char>(0 < mantissa) - static_cast<char>(mantissa < 0);
    }

    inline bool dex::isinf() const {
        return std::isinf(mantissa);
    }

    inline bool dex::isnan() const {
        return std::isnan(mantissa);
    }

    inline bool dex::is_zero() const {
        return sgn() == 0;
    }

    inline dex::operator double() const {
        return ldexp(mantissa, exp2);
    }

    inline std::string dex::to_string() const {
        // m * 2^n
        // = m * 10^(log10(2) * n)
        // = exp10 = log10(2) * n
        if (isnan()) {
            return "nan";
        }
        if (isinf()) {
            return sgn() > 0 ? "inf" : "-inf";
        }
        const double raw_exp10 = Constants::Num::LOG10_2 * exp2;
        auto exp10 = static_cast<int>(raw_exp10);
        double mantissa10 = mantissa * std::pow(10, raw_exp10 - exp10);

        while (std::abs(mantissa10) < 1 && mantissa10 != 0) {
            mantissa10 *= 10;
            exp10 -= 1;
        }
        return std::format("{}e{}", mantissa10, exp10);
    }

    inline int dex::get_exp2() const {
        return exp2;
    }

    inline double dex::get_mantissa() const {
        return mantissa;
    }

    inline void dex::try_normalize() {
        if (mantissa * sgn() > NORMALIZE_CONSTANT_MAX || mantissa * sgn() < NORMALIZE_CONSTANT_MIN) {
            normalize(this);
        }
    }

    inline dex dex::value(const double value) {
        if (value == 0) {
            return ZERO;
        }
        if (std::isinf(value)) {
            return value > 0 ? PINF : NINF;
        }
        if (std::isnan(value)) {
            return NN;
        }

        auto result = dex(0, 0);
        cpy(&result, value);
        return result;
    }
}