//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include "LightMandelbrotReference.h"
#include "MandelbrotPerturbator.h"
#include "../mrthy/LightMPATable.h"

namespace merutilm::rff2 {
    class LightMandelbrotPerturbator final : public MandelbrotPerturbator{
        std::unique_ptr<LightMandelbrotReference> reference = nullptr;
        std::unique_ptr<LightMPATable> table = nullptr;

        const double dcMax;
        const double offR;
        const double offI;

    public:

        explicit LightMandelbrotPerturbator(ParallelRenderState &state, const FractalAttribute &calc, double dcMax, int exp10,
                                   uint64_t initialPeriod, ApproxTableCache &tableRef, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                   std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                                   bool arbitraryPrecisionFPGBn = false, std::unique_ptr<LightMandelbrotReference> reusedReference = nullptr, std::unique_ptr<LightMPATable> reusedTable = nullptr,
                                   double offR = 0, double offI = 0);


        double iterate(const dex &dcr, const dex &dci) const override;

        std::unique_ptr<LightMandelbrotPerturbator> reuse(const FractalAttribute &calc, double dcMax, ApproxTableCache &tableRef);

        const LightMandelbrotReference *getReference() const override;

        LightMPATable &getTable() const;

        double getDcMax() const;

        dex getDcMaxAsDoubleExp() const override;
    };



    inline const LightMandelbrotReference *LightMandelbrotPerturbator::getReference() const {
        return reference.get();
    }

    inline LightMPATable &LightMandelbrotPerturbator::getTable() const {
        return *table;
    }

    inline double LightMandelbrotPerturbator::getDcMax() const {
        return dcMax;
    }

    inline dex LightMandelbrotPerturbator::getDcMaxAsDoubleExp() const {
        return dex::value(dcMax);
    }
}