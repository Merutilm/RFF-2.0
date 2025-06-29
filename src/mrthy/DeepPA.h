//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "ArrayCompressionTool.h"
#include "PA.h"
#include "../calc/dex.h"
#include "../formula/DeepMandelbrotReference.h"


namespace merutilm::rff2 {
    struct DeepPA final : public PA{
        const dex anr;
        const dex ani;
        const dex bnr;
        const dex bni;
        const dex radius;

        DeepPA(const dex &anr, const dex &ani, const dex &bnr, const dex &bni, uint64_t skip, const dex &radius);

        bool isValid(dex *temp, const dex &dzRad) const;

        dex getRadius() const;

    };

    inline DeepPA::DeepPA(const dex &anr, const dex &ani, const dex &bnr, const dex &bni,
                   const uint64_t skip, const dex &radius) : PA(skip), anr(anr), ani(ani), bnr(bnr), bni(bni),
                                                                    radius(radius) {
    }



    inline dex DeepPA::getRadius() const {
        return radius;
    }


    inline bool DeepPA::isValid(dex *temp, const dex &dzRad) const {
        dex::sub(temp, radius, dzRad);
        return temp->sgn() > 0;
    }
}
