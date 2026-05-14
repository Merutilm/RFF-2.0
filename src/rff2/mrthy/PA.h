//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <cstdint>
#include "../calc/calculatable.hpp"
#include "../calc/dex.h"
namespace merutilm::rff2 {

    //DO NOT MODIFY THIS FIELDS OUTER SCOPE
    template<Number Num>
    struct PA {
        uint64_t skip;
        Num anr;
        Num ani;
        Num bnr;
        Num bni;
        Num radius;

        explicit PA(const uint64_t skip, const Num anr, const Num ani, const Num bnr, const Num bni, const Num radius) : skip(skip), anr(anr), ani(ani), bnr(bnr), bni(bni), radius(radius) {
        }

        bool isValid(const Num dzRad) const {
            return dzRad < radius;
        }
    };

    using LightPA = PA<double>;
    using DeepPA = PA<dex>;
}