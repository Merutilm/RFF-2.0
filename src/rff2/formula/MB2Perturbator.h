//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "../mrthy/MPATable.h"
#include "../parallel/ParallelRenderState.h"
#include "Perturbator.h"

namespace merutilm::rff2 {

    struct MB2Perturbator : public Perturbator {

        ParallelRenderState &state;
        dex dcMax;
        dex offR = dex::ZERO;
        dex offI = dex::ZERO;

    protected:
        const FrtGeneralSettings &generalSettings;
        const FrtPerturbSettings &ptbSettings;

    public:
        explicit MB2Perturbator(ParallelRenderState &state, const dex dcMax,
        const FrtGeneralSettings &generalSettings,
        const FrtPerturbSettings &ptbSettings) : state(state), dcMax(dcMax), generalSettings(generalSettings), ptbSettings(ptbSettings) {
        }

        ~MB2Perturbator() override = default;

        virtual double iterate(dex dcr, dex dci) const = 0;
    };
}
