//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include "LightMB2Reference.h"
#include "MB2Perturbator.h"
#include "../mrthy/LightMPATable.h"

namespace merutilm::rff2 {
    class LightMB2Perturbator final : public MB2Perturbator {

        const LightMB2Reference &reference;
        const LightMPATable *table;
    public:

        explicit LightMB2Perturbator(ParallelRenderState &state, const dex dcMax,
        FrtGeneralSettings generalSettings,
        FrtPerturbSettings ptbSettings, const LightMB2Reference &reference, const LightMPATable *table) : MB2Perturbator(state, dcMax, std::move(generalSettings), std::move(ptbSettings)), reference(reference), table(table) {}

        [[nodiscard]] double iterate(dex dcr, dex dci) const override;

    };
}