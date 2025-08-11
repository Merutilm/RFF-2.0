//
// Created by Merutilm on 2025-05-11.
//

#pragma once
#include "../formula/LightMandelbrotReference.h"
#include "PA.h"

namespace merutilm::rff2 {
    struct LightPA final : public PA {
        const double anr;
        const double ani;
        const double bnr;
        const double bni;
        const double radius;

        LightPA(double anr, double ani, double bnr, double bni, uint64_t skip, double radius);

        bool isValid(double dzRad) const;
    };


    inline LightPA::LightPA(const double anr, const double ani, const double bnr, const double bni, const uint64_t skip, const double radius) : PA(skip), anr(anr), ani(ani), bnr(bnr), bni(bni), radius(radius){

    }

    inline bool LightPA::isValid(const double dzRad) const {
        return dzRad < radius;
    }
}
