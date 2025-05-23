//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include "LightMandelbrotReference.h"
#include "MandelbrotPerturbator.h"
#include "../mrthy/LightMPATable.h"

class LightMandelbrotPerturbator final : public MandelbrotPerturbator{
    std::unique_ptr<LightMandelbrotReference> reference = nullptr;
    std::unique_ptr<LightMPATable> table = nullptr;

    const double dcMax;
    const double offR;
    const double offI;

public:
    explicit LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, double dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<LightPA>> &lightTableRef, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

    explicit LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, double dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<LightPA>> &lightTableRef, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn);

    explicit LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, double dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<LightPA>> &lightTableRef, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn, std::unique_ptr<LightMandelbrotReference> reusedReference, std::unique_ptr<LightMPATable> reusedTable,
                               double offR, double offI);


    double iterate(const double_exp &dcr, const double_exp &dci) const override;

    std::unique_ptr<LightMandelbrotPerturbator> reuse(const CalculationSettings &calc, double dcMax, int exp10);

    const LightMandelbrotReference *getReference() const override;

    LightMPATable &getTable() const override;

    double getDcMax() const;

    double_exp getDcMaxExp() const override;
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

inline double_exp LightMandelbrotPerturbator::getDcMaxExp() const {
    return double_exp::value(dcMax);
}