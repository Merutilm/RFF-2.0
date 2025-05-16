//
// Created by Merutilm on 2025-05-10.
//

#pragma once

#include "LightMandelbrotReference.h"
#include "../approx/LightMPATable.h"

class LightMandelbrotPerturbator {
    ParallelRenderState &state;
    const CalculationSettings calc;
    std::unique_ptr<LightMandelbrotReference> reference = nullptr;
    std::unique_ptr<LightMPATable> table = nullptr;

    const double dcMax;
    const double offR;
    const double offI;
    const bool arbitraryPrecisionFPGBn;

public:
    LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, double dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<LightPA>> &&previousAllocatedTable, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

    LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, double dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<LightPA>> &&previousAllocatedTable, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn);

    LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, double dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<LightPA>> &&previousAllocatedTable, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn, std::unique_ptr<LightMandelbrotReference> reusedReference, std::unique_ptr<LightMPATable> reusedTable,
                               double offR, double offI);


    double iterate(double dcr, double dci) const;

    std::unique_ptr<LightMandelbrotPerturbator> reuse(const CalculationSettings &calc, double dcMax, int exp10);

    const LightMandelbrotReference *getReference() const {
        return reference.get();
    }

    const CalculationSettings &getCalculationSettings() const {
        return calc;
    }

    LightMPATable &getTable() const {
        return *table;
    }

    double getDcMax() const {
        return dcMax;
    }

};
