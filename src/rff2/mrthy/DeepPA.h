//
// Created by Merutilm on 2025-05-18.
//

#pragma once

#include "PA.h"
#include "../calc/dex.h"


namespace merutilm::rff2 {

    //DO NOT MODIFY THIS FIELDS OUTER SCOPE
    struct DeepPA final : public PA{
        dex anr;
        dex ani;
        dex bnr;
        dex bni;
        dex radius;

        explicit DeepPA(const dex anr, const dex ani, const dex bnr, const dex bni, uint64_t skip, const dex radius);

        bool isValid(dex dzRad) const;

        dex getRadius() const;

    };

    inline DeepPA::DeepPA(const dex anr, const dex ani, const dex bnr, const dex bni,
                   const uint64_t skip, const dex radius) : PA(skip), anr(anr), ani(ani), bnr(bnr), bni(bni),
                                                                    radius(radius) {
    }



    inline dex DeepPA::getRadius() const {
        return radius;
    }


    inline bool DeepPA::isValid(const dex dzRad) const {
        return dzRad < radius;
    }
}
