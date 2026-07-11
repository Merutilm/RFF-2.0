//
// Created by Merutilm on 2026-05-29.
//

#pragma once
#include "calculatable.hpp"
namespace merutilm::rff2 {

    template<Number Num>
    struct complex {
        Num re;
        Num im;

        inline static const complex ZERO = {Num(0), Num(0)};
        inline static const complex ONE = {Num(1), Num(0)};

        template<Number Num2>
        explicit operator complex<Num2>() const {
            return {static_cast<Num2>(re), static_cast<Num2>(im)};
        }

        friend complex operator*(const complex a, const complex b) {
            return complex{a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re};
        }

        friend complex &operator*=(complex &a, const Num b) { return a = a * b; }

        friend complex &operator*=(complex &a, const complex b) { return a = a * b; }

        friend complex operator*(const complex a, const Num b) { return complex{a.re * b, a.im * b}; }

        friend complex operator*(const Num a, const complex b) { return complex{a * b.re, a * b.im}; }

        friend complex operator+(const complex a, const complex b) {
            return complex{a.re + b.re, a.im + b.im};
        }

        friend complex &operator+=(complex &a, const complex b) { return a = a + b; }

        friend complex operator+(const Num a, const complex b) { return complex{a + b.re, b.im}; }

        friend complex operator+(const complex a, const Num b) { return complex{a.re + b, a.im}; }

        friend complex operator-(const complex a, const complex b) {
            return complex{a.re - b.re, a.im - b.im};
        }

        friend complex operator-(const complex a, const Num b) { return complex{a.re - b, a.im}; }

        friend complex operator-(const Num a, const complex b) { return complex{a - b.re, -b.im}; }


        [[nodiscard]] Num norm_sqr() const { return re * re + im * im; }

        [[nodiscard]] Num norm_approx() const { return calculatable::hypot_approx(re, im); }

        [[nodiscard]] bool is_zero() const { return calculatable::is_zero(re) && calculatable::is_zero(im); }

        [[nodiscard]] complex try_normalized_value() const {
            return complex{calculatable::try_normalized_value(re), calculatable::try_normalized_value(im)};
        }
        std::string to_string() {
            if constexpr (std::is_same_v<Num, dex>) {
                return re.to_string() + " | " + im.to_string() + "i";
            }else {
                static_assert(std::is_same_v<Num, double>);
                return std::to_string(re) + " | " + std::to_string(im) + "i";
            }
        }
    };
} // namespace merutilm::rff2
