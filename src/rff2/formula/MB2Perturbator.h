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
        FrtGeneralSettings generalSettings;
        FrtPerturbSettings ptbSettings;
        dex offR = dex::ZERO;
        dex offI = dex::ZERO;

        explicit MB2Perturbator(ParallelRenderState &state, const dex dcMax,
        FrtGeneralSettings generalSettings,
        FrtPerturbSettings ptbSettings) : state(state), dcMax(dcMax), generalSettings(std::move(generalSettings)), ptbSettings(std::move(ptbSettings)) {
        }

        ~MB2Perturbator() override = default;

        virtual double iterate(dex dcr, dex dci) const = 0;
    };
}
