//
// Created by Merutilm on 2025-05-17.
//

#pragma once
#include <cmath>
#include <format>
#include <numbers>
#include <string>

#include "../constants/Constants.hpp"
#include "templates.hpp"

namespace merutilm::rff2 {

    template<Number T>
    struct exp_traits;

    template<>
    struct exp_traits<double> {
        static constexpr uint64_t EXP_MASK = 0x7ff0000000000000ULL;
        static constexpr uint64_t EXP_MANTISSA_MASK = 0x7fffffffffffffffULL;
        static constexpr uint64_t SGN_MANTISSA_MASK = 0x800fffffffffffffULL;
        static constexpr uint64_t NORMALIZED_EXP_BITS = 0x3fe0000000000000ULL;
        static constexpr int NORMALIZED_EXP_SUB = 0x03fe;
        static constexpr double NORMALIZE_CONSTANT_MAX = 1e75;
        static constexpr double NORMALIZE_CONSTANT_MIN = 1e-75;
        static constexpr int MANTISSA_BIT_COUNT = 52;
    };

    template<>
    struct exp_traits<float> {
        static constexpr uint32_t EXP_MASK = 0x7f800000U;
        static constexpr uint32_t EXP_MANTISSA_MASK = 0x7fffffffU;
        static constexpr uint32_t SGN_MANTISSA_MASK = 0x807fffffU;
        static constexpr uint32_t NORMALIZED_EXP_BITS = 0x3f000000U;
        static constexpr int NORMALIZED_EXP_SUB = 0x007e;
        static constexpr float NORMALIZE_CONSTANT_MAX = 1e9;
        static constexpr float NORMALIZE_CONSTANT_MIN = 1e-9;
        static constexpr int MANTISSA_BIT_COUNT = 23;
    };

    /**
     * the floating-point object which supports semi-infinity exponents.
     */
    template<Number Exp, Number Mantissa, Number Bit>
    struct exponent {
        static_assert(sizeof(Exp) == sizeof(Mantissa) && sizeof(Mantissa) == sizeof(Bit));

        Exp exp2;
        Mantissa mantissa;

        static const exponent ZERO;
        static const exponent ONE;

#ifdef SAFE_EXP_OPERATOR
        static const exponent NN;
        static const exponent PINF;
        static const exponent NINF;
#endif


        constexpr exponent() noexcept : exponent(0, 0) {}


        constexpr explicit exponent(const Exp exp2, const Mantissa mantissa) noexcept : exp2(exp2), mantissa(mantissa) {}


        constexpr explicit exponent(const Mantissa value) noexcept : exp2(0), mantissa(value) {}


        static Mantissa ldexp_neg(const Mantissa mantissa, const Exp exp2) {
            const auto mts_bits = std::bit_cast<Bit>(mantissa);
            const auto mts_ubits = mts_bits & exp_traits<Mantissa>::EXP_MANTISSA_MASK;
            const auto f_shift = static_cast<int>(mts_ubits >> exp_traits<Mantissa>::MANTISSA_BIT_COUNT) + exp2;
            // do not consider < smallest normalized value
            return f_shift < 0 ? 0
                               : std::bit_cast<Mantissa>(mts_bits - (static_cast<Bit>(-exp2)
                                                                   << exp_traits<Mantissa>::MANTISSA_BIT_COUNT));
        }


        static exponent sqrt(const exponent v) { return exponent{v.exp2 >> 1, v.sgn() * std::sqrt(std::abs(v.mantissa))}; }


        static exponent nthRoot(const exponent v, const int d) {
            // valid when d < 32, v.mantissa > 0
            const int64_t k = (v.exp2 % d + d) % d;
            const int64_t exp2 = v.exp2 - k;
            Mantissa mantissa = std::pow(v.mantissa * (1u << k), 1.0 / d);
            return exponent{exp2 / d, mantissa};
        }


        static exponent mul_2exp(const exponent v, const int exp2) { return exponent{v.exp2 + exp2, v.mantissa}; }


        static exponent div_2exp(const exponent v, const int exp2) { return exponent{v.exp2 - exp2, v.mantissa}; }

        explicit operator double() const { return std::ldexp(mantissa, static_cast<int>(exp2)); }

        explicit operator float() const { return static_cast<float>(std::ldexp(mantissa, static_cast<int>(exp2))); }

        template<Number ExpCast, Number MantissaCast, Number BitCast>
        explicit operator exponent<ExpCast, MantissaCast, BitCast>() const {
            return exponent<ExpCast, MantissaCast, BitCast>{static_cast<ExpCast>(exp2), static_cast<MantissaCast>(mantissa)};
        }

        friend exponent operator-(const exponent a) { return exponent{a.exp2, -a.mantissa}; }

        friend exponent operator+(const exponent a, const exponent b) {

#ifdef SAFE_EXP_OPERATOR

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
            const Exp d_exp2 = a.exp2 - b.exp2;
            return exponent{std::max(a.exp2, b.exp2), ldexp_neg(a.mantissa, std::min(static_cast<Exp>(0), d_exp2)) +
                                                      ldexp_neg(b.mantissa, std::min(static_cast<Exp>(0), -d_exp2))};
        }
        friend exponent operator+(const exponent a, const Mantissa b) { return a + exponent(b); }

        friend exponent operator+(const Mantissa a, const exponent b) { return exponent(a) + b; }

        friend exponent operator-(const exponent a, const exponent b) {
#ifdef SAFE_EXP_OPERATOR
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
            const Exp d_exp2 = a.exp2 - b.exp2;
            return exponent{std::max(a.exp2, b.exp2), ldexp_neg(a.mantissa, std::min(static_cast<Exp>(0), d_exp2)) -
                                                      ldexp_neg(b.mantissa, std::min(static_cast<Exp>(0), -d_exp2))};
        }


        friend exponent operator-(const exponent a, const Mantissa b) { return a - exponent(b); }

        friend exponent operator-(const Mantissa a, const exponent b) { return exponent(a) - b; }

        friend exponent operator*(const exponent a, const Mantissa b) {
#ifdef SAFE_EXP_OPERATOR
            if (a.isnan() || std::isnan(b)) {
                return NN;
            }
            if (a.isinf() || std::isinf(b)) {
                return a.sgn() == (b > 0) ? PINF : NINF;
            }
#endif
            return exponent{a.exp2, a.mantissa * b};
        }

        friend exponent operator*(const Mantissa a, const exponent b) {
#ifdef SAFE_EXP_OPERATOR
            if (std::isnan(a) || b.isnan()) {
                return NN;
            }
            if (std::isinf(a) || b.isinf()) {
                return (a > 0) == b.sgn() ? PINF : NINF;
            }
#endif
            return exponent{b.exp2, a * b.mantissa};
        }

        friend exponent operator*(const exponent a, const exponent b) {
#ifdef SAFE_EXP_OPERATOR
            if (a.isnan() || b.isnan()) {
                return NN;
            }
            if (a.isinf() || b.isinf()) {
                return a.sgn() == b.sgn() ? PINF : NINF;
            }
#endif
            return exponent{a.exp2 + b.exp2, a.mantissa * b.mantissa};
        }

        friend exponent operator/(const exponent a, const exponent b) {
#ifdef SAFE_EXP_OPERATOR
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
            return exponent{a.exp2 - b.exp2, a.mantissa / b.mantissa};
        }

        friend exponent &operator+=(exponent &a, const exponent b) { return a = a + b; }

        friend exponent &operator-=(exponent &a, const exponent b) { return a = a - b; }

        friend exponent &operator*=(exponent &a, const exponent b) { return a = a * b; }

        friend exponent &operator/=(exponent &a, const exponent b) { return a = a / b; }

        bool operator==(const exponent &other) const { return (*this - other).is_zero(); }

        friend std::partial_ordering operator<=>(const exponent a, const exponent b) {
            const exponent v = a - b;
#ifdef SAFE_EXP_OPERATOR
            if (v.isnan()) {
                return std::partial_ordering::unordered;
            }
#endif
            return v.sgn() <=> 0;
        }


        void normalize() {

            if (mantissa == 0) {
                exp2 = ZERO.exp2;
                return;
            }

#ifdef SAFE_EXP_OPERATOR
            const char sgn = this->sgn();
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

            const auto mts_bits = std::bit_cast<Bit>(mantissa);
            mantissa = std::bit_cast<Mantissa>(mts_bits & exp_traits<Mantissa>::SGN_MANTISSA_MASK |
                                               exp_traits<Mantissa>::NORMALIZED_EXP_BITS);
            exp2 += static_cast<Exp>((mts_bits & exp_traits<Mantissa>::EXP_MASK) >>
                                     exp_traits<Mantissa>::MANTISSA_BIT_COUNT) -
                    exp_traits<Mantissa>::NORMALIZED_EXP_SUB;
        }


        [[nodiscard]] int sgn() const { return static_cast<int>(0 < mantissa) - static_cast<int>(mantissa < 0); }

#ifdef SAFE_EXP_OPERATOR


        bool isinf() const { return std::isinf(mantissa); }


        bool isnan() const { return std::isnan(mantissa); }
#endif


        [[nodiscard]] bool is_zero() const { return mantissa == 0; }


        [[nodiscard]] std::string to_string() const {


#ifdef SAFE_EXP_OPERATOR
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


        Exp get_exp2() const { return exp2; }


        Mantissa get_mantissa() const { return mantissa; }


        void try_normalize() {
            const auto mts = mantissa * sgn();
            if (mts > exp_traits<Mantissa>::NORMALIZE_CONSTANT_MAX ||
                mts < exp_traits<Mantissa>::NORMALIZE_CONSTANT_MIN) {
                normalize();
            }
        }
    };

    template<Number Exp, Number Mantissa, Number Bit>
    inline const exponent<Exp, Mantissa, Bit> exponent<Exp, Mantissa, Bit>::ZERO = exponent{0, 0};

    template<Number Exp, Number Mantissa, Number Bit>
    inline const exponent<Exp, Mantissa, Bit> exponent<Exp, Mantissa, Bit>::ONE = exponent{0, 1};

#ifdef SAFE_EXP_OPERATOR

    template<Number Exp, Number Mantissa, Number Bit>
    inline const exponent<Exp, Mantissa, Bit> exponent<Exp, Mantissa, Bit>::NN = exponent{0, static_cast<Mantissa>(NAN)};

    template<Number Exp, Number Mantissa, Number Bit>
    inline const exponent<Exp, Mantissa, Bit> exponent<Exp, Mantissa, Bit>::PINF = exponent{0, static_cast<Mantissa>(INFINITY)};

    template<Number Exp, Number Mantissa, Number Bit>
    inline const exponent<Exp, Mantissa, Bit> exponent<Exp, Mantissa, Bit>::NINF = exponent{0, -static_cast<Mantissa>(INFINITY)};
#endif


    using dex = exponent<int64_t, double, uint64_t>;
    using fex = exponent<int32_t, float, uint32_t>;

} // namespace merutilm::rff2
