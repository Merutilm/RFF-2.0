//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "../mrthy/MPATable.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FractalSettings.h"
#include "MB2Reference.h"
#include "Perturbator.h"

namespace merutilm::rff2 {
    struct MB2Perturbator : public Perturbator {

        ParallelRenderState &state;
        const FractalSettings calc;
        Reference::CreationResult referenceCreationResult = Reference::CreationResult::FAILED;

        explicit MB2Perturbator(ParallelRenderState &state,
                                       FractalSettings calculationSettings) : state(state),
            calc(std::move(calculationSettings)) {
        }

        ~MB2Perturbator() override = default;

        [[nodiscard]] virtual const MB2Reference *getReference() const = 0;

        [[nodiscard]] const FractalSettings &getCalculationSettings() const {
            return calc;
        };

        [[nodiscard]] virtual dex getDcMaxAsDoubleExp() const = 0;

        [[nodiscard]] virtual double iterate(const dex &dcr, const dex &dci) const = 0;
    };
}
