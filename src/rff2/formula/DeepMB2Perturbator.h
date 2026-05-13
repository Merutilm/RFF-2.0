//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "../mrthy/DeepMPATable.h"
#include "DeepMB2Reference.h"
#include "MB2Perturbator.h"


namespace merutilm::rff2 {
    class DeepMB2Perturbator final : public MB2Perturbator {
        const DeepMB2Reference &reference;
        const DeepMPATable *table;

    public:
        explicit DeepMB2Perturbator(ParallelRenderState &state, const dex dcMax, FrtGeneralSettings generalSettings,
                                    FrtPerturbSettings ptbSettings, const DeepMB2Reference &reference,
                                    const DeepMPATable *table) :
            MB2Perturbator(state, dcMax, std::move(generalSettings), std::move(ptbSettings)), reference(reference),
            table(table) {}

        [[nodiscard]] double iterate(dex dcr, dex dci) const override;
    };

} // namespace merutilm::rff2
