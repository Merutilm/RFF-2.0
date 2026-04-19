//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include "LightMB2Reference.h"
#include "MB2Perturbator.h"
#include "../mrthy/LightMPATable.h"

namespace merutilm::rff2 {
    class LightMB2Perturbator final : public MB2Perturbator{
        std::unique_ptr<LightMB2Reference> reference = nullptr;
        std::unique_ptr<LightMPATable> table = nullptr;

        const double dcMax;
        const double offR;
        const double offI;

    public:

        explicit LightMB2Perturbator(ParallelRenderState &state, const FractalSettings &calc, double dcMax, int exp10,
                                   uint64_t initialPeriod, ApproxTableCache &tableRef, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                   std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                                   bool arbitraryPrecisionFPGBn = false, std::unique_ptr<LightMB2Reference> reusedReference = nullptr, std::unique_ptr<LightMPATable> reusedTable = nullptr,
                                   double offR = 0, double offI = 0);


        [[nodiscard]] double iterate(const dex &dcr, const dex &dci) const override;

        [[nodiscard]] std::unique_ptr<LightMB2Perturbator> reuse(const FractalSettings &calc, double dcMax, ApproxTableCache &tableRef);

        [[nodiscard]] const LightMB2Reference *getReference() const override;

        [[nodiscard]] LightMPATable &getTable() const;

        [[nodiscard]] double getDcMax() const;

        [[nodiscard]] dex getDcMaxAsDoubleExp() const override;
    };



    inline const LightMB2Reference *LightMB2Perturbator::getReference() const {
        return reference.get();
    }

    inline LightMPATable &LightMB2Perturbator::getTable() const {
        return *table;
    }

    inline double LightMB2Perturbator::getDcMax() const {
        return dcMax;
    }

    inline dex LightMB2Perturbator::getDcMaxAsDoubleExp() const {
        return dex::value(dcMax);
    }
}