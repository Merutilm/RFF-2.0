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

#ifndef NDEBUG
        uint64_t start;
#endif

        uint64_t skip;
        Num radius;
        complex<Num> an;
        complex<Num> bn;

        explicit PA() = default;

#ifndef NDEBUG
        explicit PA(const uint64_t start, const uint64_t skip, const complex<Num> an, const complex<Num> bn, const Num radius) : start(start), skip(skip), radius(radius), an(an), bn(bn) {
        }
#else
        explicit PA(const uint64_t skip, const complex<Num> an, const complex<Num> bn, const Num radius) : skip(skip), radius(radius), an(an), bn(bn) {
        }
#endif

        [[nodiscard]] complex<Num> apply(const complex<Num> dz, const complex<Num> dc) const {
            return an * dz + bn * dc;
        }

        [[nodiscard]] bool isValid(const Num dzRad) const {
            return dzRad < radius;
        }
    };

    using LightPA = PA<double>;
    using DeepPA = PA<dex>;
}