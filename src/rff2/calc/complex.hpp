//
// Created by Merutilm on 2026-05-29.
//

#pragma once
#include "rff_math.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    struct complex {
        Num re;
        Num im;

        static const complex ZERO;
        static const complex ONE;

        template<Number NCast>
        explicit operator complex<NCast>() const {
            return {NCast(re), NCast(im)};
        }

        friend complex operator*(const complex a, const complex b) {
            return complex{.re = a.re * b.re - a.im * b.im, .im = a.re * b.im + a.im * b.re};
        }


        template<Number N2>
        friend complex &operator*=(complex &a, const N2 b) { return a = a * b; }

        friend complex &operator*=(complex &a, const complex b) { return a = a * b; }

        template<Number N2>
        friend complex operator*(const complex a, const N2 b) { return complex{a.re * b, a.im * b}; }

        template<Number N2>
        friend complex operator*(const N2 a, const complex b) { return complex{a * b.re, a * b.im}; }

        friend complex operator+(const complex a, const complex b) { return complex{a.re + b.re, a.im + b.im}; }

        friend complex &operator+=(complex &a, const complex b) { return a = a + b; }

        template<Number N2>
        friend complex operator+(const N2 a, const complex b) { return complex{.re = a + b.re, .im = b.im}; }

        template<Number N2>
        friend complex operator+(const complex a, const N2 b) { return complex{.re = a.re + b, .im = a.im}; }

        friend complex operator-(const complex a, const complex b) { return complex{.re = a.re - b.re, .im = a.im - b.im}; }

        friend complex &operator-=(complex &a, const complex b) { return a = a - b; }

        template<Number N2>
        friend complex operator-(const complex a, const N2 b) { return complex{.re = a.re - b, .im = a.im}; }

        template<Number N2>
        friend complex operator-(const N2 a, const complex b) { return complex{.re = a - b.re, .im = -b.im}; }

        template<Number N2>
        friend complex operator/(const complex a, const N2 b) { return complex{.re = a.re / b, .im = a.im / b}; }

        friend bool operator==(const complex &a, const complex &b) = default;

        [[nodiscard]] Num norm_sqr() const { return re * re + im * im; }

        [[nodiscard]] Num norm_approx() const { return rff_math::hypot_approx(re, im); }

        [[nodiscard]] bool is_zero() const { return rff_math::is_zero(re) && rff_math::is_zero(im); }

        [[nodiscard]] complex try_normalized_value() const {
            return complex{rff_math::try_normalized_value(re), rff_math::try_normalized_value(im)};
        }
        [[nodiscard]] std::string to_string() const {
            if constexpr (rff_math::is_exponent<Num>) {
                return re.to_string() + " | " + im.to_string() + "i";
            } else if constexpr (std::is_same_v<Num, double>) {
                return std::format("{:.15e}", re) + " | " + std::format("{:.15e}",im) + "i";
            } else {
                static_assert(std::is_same_v<Num, float>);
                return std::format("{:.6e}", re) + " | " + std::format("{:.6e}",im) + "i";
            }
        }
        friend std::ostream &operator<<(std::ostream &out, const complex &obj) {
            return out << obj.to_string();
        }
    };
    template<Number Num>
    constexpr complex<Num> complex<Num>::ZERO = {.re = Num(0), .im = Num(0)};
    template<Number Num>
    constexpr complex<Num> complex<Num>::ONE = {.re = Num(1), .im = Num(0)};

} // namespace merutilm::rff2
