//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "DeepMB2Reference.h"
#include "MB2Perturbator.h"
#include "../mrthy/DeepMPATable.h"

namespace merutilm::rff2 {
    class DeepMB2Perturbator final : public MB2Perturbator {

        std::unique_ptr<DeepMB2Reference> reference = nullptr;
        std::unique_ptr<DeepMPATable> table = nullptr;

        const dex dcMax;
        const dex offR;
        const dex offI;

    public:

        explicit DeepMB2Perturbator(ParallelRenderState &state, const FractalSettings &calc,
                                           const dex &dcMax, int exp10,
                                           uint64_t refInitialCapacity, uint64_t fixedPeriod, ApproxTableManager &tableRef,
                                           std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                           std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                                           bool arbitraryPrecisionFPGBn = false,
                                           std::unique_ptr<DeepMB2Reference> reusedReference = nullptr,
                                           std::unique_ptr<DeepMPATable> reusedTable = nullptr,
                                           const dex &offR = dex::ZERO, const dex &offI = dex::ZERO);


        [[nodiscard]] double iterate(const dex &dcr, const dex &dci) const override;

        std::unique_ptr<DeepMB2Perturbator> reuse(const FractalSettings &calc, const dex &dcMax,
                                                         ApproxTableManager &tableRef);

        [[nodiscard]] const DeepMB2Reference *getReference() const override;

        [[nodiscard]] DeepMPATable &getTable() const;

        [[nodiscard]] dex getDcMaxAsDoubleExp() const override;
    };

    // DEFINITION OF DEEP MANDELBROT PERTURBATOR

    inline const DeepMB2Reference *DeepMB2Perturbator::getReference() const {
        return reference.get();
    }

    inline DeepMPATable &DeepMB2Perturbator::getTable() const {
        return *table;
    }

    inline dex DeepMB2Perturbator::getDcMaxAsDoubleExp() const {
        return dcMax;
    }
}
