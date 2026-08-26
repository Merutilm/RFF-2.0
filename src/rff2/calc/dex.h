//
// Created by Merutilm on 2025-05-17.
//

#pragma once
#include <cmath>
#include <format>
#include <string>

#include "../constants/Constants.hpp"

namespace merutilm::rff2 {

    /**
     * the floating-point object which supports semi-infinity exponents.
     */
    struct dex {
        int64_t exp2;
        double mantissa;

        static const dex ZERO;
        static const dex ONE;

#ifdef SAFE_DEX_OPERATOR
        static const dex NN;
        static const dex PINF;
        static const dex NINF;
#endif

        static constexpr double NORMALIZE_CONSTANT_MAX = 1e75;
        static constexpr double NORMALIZE_CONSTANT_MIN = 1e-75;

        constexpr dex() noexcept;

        constexpr dex(int64_t exp2, double mantissa) noexcept;

        explicit constexpr dex(double value) noexcept;

        static double ldexp_neg(double mantissa, int64_t exp2);

        [[nodiscard]] static dex sqrt(dex v);

        [[nodiscard]] static dex nthRoot(dex v, int d);

        static dex mul_2exp(dex v, int exp2);

        static dex div_2exp(dex v, int exp2);

        explicit operator double() const;

        explicit operator float() const;

        bool operator==(const dex &other) const;

        friend dex operator-(const dex a) { return {a.exp2, -a.mantissa}; }

        friend dex operator+(const dex a, const dex b) {

#ifdef SAFE_DEX_OPERATOR

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
#else
            if (b.is_zero()) [[unlikely]] {
                return a;
            }
            if (a.is_zero()) [[unlikely]] {
                return b;
            }
#endif
            const int64_t d_exp2 = a.exp2 - b.exp2;
            return {std::max(a.exp2, b.exp2),
                    ldexp_neg(a.mantissa, std::min(static_cast<int64_t>(0), d_exp2)) +
                            ldexp_neg(b.mantissa, std::min(static_cast<int64_t>(0), -d_exp2))};
        }
        friend dex operator+(const dex a, const double b) { return a + dex(b); }

        friend dex operator+(const double a, const dex b) { return dex(a) + b; }

        friend dex operator-(const dex a, const dex b) {
#ifdef SAFE_DEX_OPERATOR
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
#else

            if (b.is_zero()) [[unlikely]] {
                return a;
            }
            if (a.is_zero()) [[unlikely]] {
                return -b;
            }
#endif
            const int64_t d_exp2 = a.exp2 - b.exp2;
            return {std::max(a.exp2, b.exp2),
                    ldexp_neg(a.mantissa, std::min(static_cast<int64_t>(0), d_exp2)) -
                            ldexp_neg(b.mantissa, std::min(static_cast<int64_t>(0), -d_exp2))};
        }


        friend dex operator-(const dex a, const double b) { return a - dex(b); }

        friend dex operator-(const double a, const dex b) { return dex(a) - b; }

        friend dex operator*(const dex a, const double b) {
#ifdef SAFE_DEX_OPERATOR
            if (a.isnan() || std::isnan(b)) {
                return NN;
            }
            if (a.isinf() || std::isinf(b)) {
                return a.sgn() == (b > 0) ? PINF : NINF;
            }
#endif
            return {a.exp2, a.mantissa * b};
        }

        friend dex operator*(const double a, const dex b) {
#ifdef SAFE_DEX_OPERATOR
            if (std::isnan(a) || b.isnan()) {
                return NN;
            }
            if (std::isinf(a) || b.isinf()) {
                return (a > 0) == b.sgn() ? PINF : NINF;
            }
#endif
            return {b.exp2, a * b.mantissa};
        }

        friend dex operator*(const dex a, const dex b) {
#ifdef SAFE_DEX_OPERATOR
            if (a.isnan() || b.isnan()) {
                return NN;
            }
            if (a.isinf() || b.isinf()) {
                return a.sgn() == b.sgn() ? PINF : NINF;
            }
#endif
            return {a.exp2 + b.exp2, a.mantissa * b.mantissa};
        }

        friend dex operator/(const dex a, const dex b) {
#ifdef SAFE_DEX_OPERATOR
            if (a.is_zero() && b.is_zero()) {
                return NN;
            }
            if (a.is_zero() || b.isinf()) {
                return ZERO;
            }
            if (b.is_zero() || a.isinf()) {
                return a.sgn() == b.sgn() ? PINF : NINF;
            }
#endif
            return {a.exp2 - b.exp2, a.mantissa / b.mantissa};
        }

        friend dex &operator+=(dex &a, const dex b) { return a = a + b; }

        friend dex &operator-=(dex &a, const dex b) { return a = a - b; }

        friend dex &operator*=(dex &a, const dex b) { return a = a * b; }

        friend dex &operator/=(dex &a, const dex b) { return a = a / b; }

        friend std::partial_ordering operator<=>(const dex a, const dex b) {
            const dex v = a - b;
#ifdef SAFE_DEX_OPERATOR
            if (v.isnan()) {
                return std::partial_ordering::unordered;
            }
#endif
            return v.sgn() <=> 0;
        }


        void normalize();

        [[nodiscard]] int sgn() const;

#ifdef SAFE_DEX_OPERATOR
        [[nodiscard]] bool isinf() const;

        [[nodiscard]] bool isnan() const;
#endif

        [[nodiscard]] bool is_zero() const;


        [[nodiscard]] std::string to_string() const;

        [[nodiscard]] int64_t get_exp2() const;

        [[nodiscard]] double get_mantissa() const;

        void try_normalize();
    };

    // DEFINITION OF DOUBLE EXP
    // add definition on header to improve performance by 20-30%


    inline const dex dex::ZERO = {0, 0};
    inline const dex dex::ONE = {0, 1};

#ifdef SAFE_DEX_OPERATOR
    inline const dex dex::NN = {0, NAN};
    inline const dex dex::PINF = {0, INFINITY};
    inline const dex dex::NINF = {0, -static_cast<double>(INFINITY)};
#endif
    constexpr dex::dex() noexcept : dex(0, 0) {}

    constexpr dex::dex(const int64_t exp2, const double mantissa) noexcept : exp2(exp2), mantissa(mantissa) {}

    constexpr dex::dex(const double value) noexcept : exp2(0), mantissa(value) {}

    inline double dex::ldexp_neg(const double mantissa, const int64_t exp2) {
        const auto mts_bits = std::bit_cast<uint64_t>(mantissa);
        const auto mts_ubits = mts_bits & 0x7fffffffffffffffULL;
        const auto f_shift = static_cast<int>(mts_ubits >> 52) + exp2;
        // do not consider < e-309
        return f_shift < 0 ? 0 : std::bit_cast<double>(mts_bits - (static_cast<uint64_t>(-exp2) << 52));
    }

    inline dex dex::sqrt(const dex v) { return {v.exp2 >> 1, v.sgn() * std::sqrt(std::abs(v.mantissa))}; }

    inline dex dex::nthRoot(const dex v, const int d) {
        // valid when d < 32, v.mantissa > 0
        const int64_t k = (v.exp2 % d + d) % d;
        const int64_t exp2 = v.exp2 - k;
        double mantissa = pow(v.mantissa * (1u << k), 1.0 / d);
        return {exp2 / d, mantissa};
    }

    inline dex dex::mul_2exp(const dex v, const int exp2) { return {v.exp2 + exp2, v.mantissa}; }

    inline dex dex::div_2exp(const dex v, const int exp2) { return {v.exp2 - exp2, v.mantissa}; }


    inline void dex::normalize() {

        if (mantissa == 0) {
            exp2 = ZERO.exp2;
            return;
        }

#ifdef SAFE_DEX_OPERATOR
        const char sgn = dex::sgn();
        if (isinf()) {
            if (sgn == 1) {
                exp2 = PINF.exp2;
                mantissa = PINF.mantissa;
            } else {
                exp2 = NINF.exp2;
                mantissa = NINF.mantissa;
            }
            return;
        }
        if (isnan()) {
            exp2 = NN.exp2;
            mantissa = NN.mantissa;
            return;
        }
#endif

        const auto mts_bits = std::bit_cast<uint64_t>(mantissa);
        mantissa = std::bit_cast<double>(mts_bits & 0x800fffffffffffffULL | 0x3fe0000000000000ULL);
        exp2 += static_cast<int>((mts_bits & 0x7ff0000000000000ULL) >> 52) - 0x03fe;
    }

    inline int dex::sgn() const { return static_cast<int>(0 < mantissa) - static_cast<int>(mantissa < 0); }

#ifdef SAFE_DEX_OPERATOR
    inline bool dex::isinf() const { return std::isinf(mantissa); }

    inline bool dex::isnan() const { return std::isnan(mantissa); }
#endif

    inline bool dex::is_zero() const { return mantissa == 0; }

    inline dex::operator double() const { return ldexp(mantissa, static_cast<int>(exp2)); }

    inline dex::operator float() const { return static_cast<float>(ldexp(mantissa, static_cast<int>(exp2))); }

    inline std::string dex::to_string() const {


#ifdef SAFE_DEX_OPERATOR
        if (isnan()) {
            return "nan";
        }
        if (isinf()) {
            return sgn() > 0 ? "inf" : "-inf";
        }
#endif

        if (mantissa == 0.0) {
            return "0";
        }
        // m * 2^n
        // = m * 10^(log10(2) * n)
        // = exp10 = log10(2) * n
        //
        const double raw_exp10 = std::numbers::ln2 / std::numbers::ln10 * static_cast<double>(exp2);
        auto exp10 = static_cast<int>(raw_exp10);
        double mantissa10 = mantissa * std::pow(10, raw_exp10 - exp10);
        const double abs_m = std::abs(mantissa10);

        if (abs_m >= 10.0) {
            const int shift = static_cast<int>(std::floor(std::log10(abs_m)));
            mantissa10 *= std::pow(10.0, -shift);
            exp10 += shift;
        } else if (abs_m < 1.0) {
            const int shift = static_cast<int>(std::ceil(-std::log10(abs_m)));
            mantissa10 *= std::pow(10.0, shift);
            exp10 -= shift;
        }


        return std::format("{}e{}", mantissa10, exp10);
    }

    inline int64_t dex::get_exp2() const { return exp2; }

    inline double dex::get_mantissa() const { return mantissa; }

    inline void dex::try_normalize() {
        const auto mts = mantissa * sgn();
        if (mts > NORMALIZE_CONSTANT_MAX || mts < NORMALIZE_CONSTANT_MIN) {
            normalize();
        }
    }
} // namespace merutilm::rff2
