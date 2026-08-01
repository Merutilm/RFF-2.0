//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "../calc/calculatable.hpp"
#include "../calc/complex.hpp"
#include "../calc/dex.h"
namespace merutilm::rff2 {

    template<Number Num>
    struct PA {

        uint64_t skip;
        Num radius;
        complex<Num> an;
        complex<Num> bn;

        explicit PA(const uint64_t skip, const complex<Num> an, const complex<Num> bn, const Num radius) : skip(skip), radius(radius), an(an), bn(bn) {
        }


        [[nodiscard]] complex<Num> apply(const complex<Num> dz, const complex<Num> dc) {
            return an * dz + bn * dc;
        };

        [[nodiscard]] bool isValid(const Num dzRad) const {
            return dzRad < radius;
        }
    };

    using LightPA = PA<double>;
    using DeepPA = PA<dex>;
}