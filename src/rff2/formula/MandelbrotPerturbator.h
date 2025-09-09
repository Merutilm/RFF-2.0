//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "MandelbrotReference.h"
#include "Perturbator.h"
#include "../mrthy/MPATable.h"
#include "../parallel/ParallelRenderState.h"
#include "../attr/FractalAttribute.h"

namespace merutilm::rff2 {
    struct MandelbrotPerturbator : public Perturbator {
        ParallelRenderState &state;
        const FractalAttribute calc;

        explicit MandelbrotPerturbator(ParallelRenderState &state,
                                       const FractalAttribute &calculationSettings) : state(state),
            calc(calculationSettings) {
        }

        ~MandelbrotPerturbator() override = default;

        virtual const MandelbrotReference *getReference() const = 0;

        const FractalAttribute &getCalculationSettings() const {
            return calc;
        };

        virtual dex getDcMaxAsDoubleExp() const = 0;

        virtual double iterate(const dex &dcr, const dex &dci) const = 0;
    };
}
