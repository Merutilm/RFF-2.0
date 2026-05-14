//
// Created by Merutilm on 2026-05-15.
//

#pragma once
#include <concepts>
#include "dex.h"
#include "dex_exp.h"
#include "dex_trig.h"
#include "fixed_point_decimal.hpp"
#include "rff_math.h"
namespace merutilm::rff2 {

    template<typename T>
    concept Number = std::totally_ordered<T> && std::is_trivially_copyable_v<T> && requires(T a, T b) {
        { a + b } -> std::same_as<T>;
        { a - b } -> std::same_as<T>;
        { a *b } -> std::same_as<T>;
        { a / b } -> std::same_as<T>;
    };


    namespace calculatable {
        template<Number Num>
        bool is_zero(Num n) {
            if constexpr (std::is_same_v<Num, double>) {
                return n == 0;
            } else {
                static_assert(std::is_same_v<Num, dex>);
                return n.is_zero();
            }
        }

        template<Number Num>
        Num hypot_approx(Num a, Num b) {
            if constexpr (std::is_same_v<Num, double>) {
                return rff_math::hypot_approx(a, b);
            } else {
                static_assert(std::is_same_v<Num, dex>);
                return dex_trig::hypot_approx(a, b);
            }
        }

        template<Number Num>
        Num try_normalized_value(Num n) {
            if constexpr (std::is_same_v<Num, double>) {
                return n;
            } else {
                static_assert(std::is_same_v<Num, dex>);
                n.try_normalize();
                return n;
            }
        }

        template<Number Num>
        Num from_fixed_point_decimal(fixed_point_decimal &n) {
            if constexpr (std::is_same_v<Num, double>) {
                return n.double_value();
            } else {
                static_assert(std::is_same_v<Num, dex>);
                return n.dex_value();
            }
        }

    } // namespace calculatable

} // namespace merutilm::rff2
