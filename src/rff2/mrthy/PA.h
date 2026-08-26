//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "../calc/complex.hpp"
#include "../calc/dex.h"
namespace merutilm::rff2 {

    template<Number Num>
    struct PA {

        uint64_t skip;
        complex<Num> an;
        complex<Num> bn;
        Num radius;
        // For Num = float, the data members occupy 28 bytes.
        // With 8-byte alignment, sizeof(PA<float>) is 32 bytes due to 4 bytes of tail padding.
        // No explicit padding member is required.


        explicit PA() = default;

        explicit PA(const uint64_t skip, const complex<Num> an, const complex<Num> bn, const Num radius) : skip(skip), an(an), bn(bn), radius(radius) {
        }

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