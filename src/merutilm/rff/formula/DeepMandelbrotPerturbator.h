//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "DeepMandelbrotReference.h"
#include "MandelbrotPerturbator.h"
#include "../mrthy/DeepMPATable.h"
#include "../mrthy/DeepPA.h"
#include "../mrthy/LightPA.h"

class DeepMandelbrotPerturbator final : public MandelbrotPerturbator{
    std::unique_ptr<DeepMandelbrotReference> reference = nullptr;
    std::unique_ptr<DeepMPATable> table = nullptr;

    const double_exp dcMax;
    const double_exp offR;
    const double_exp offI;

public:
    explicit DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, const double_exp &dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<DeepPA>> &deepTableRef, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration);

    explicit DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, const double_exp &dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<DeepPA>> &deepTableRef, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn);

    explicit DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc, const double_exp &dcMax, int exp10,
                               uint64_t initialPeriod, std::vector<std::vector<DeepPA>> &deepTableRef, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                               std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                               bool arbitraryPrecisionFPGBn, std::unique_ptr<DeepMandelbrotReference> reusedReference, std::unique_ptr<DeepMPATable> reusedTable,
                               const double_exp &offR, const double_exp &offI);


    double iterate(const double_exp &dcr, const double_exp &dci) const override;

    std::unique_ptr<DeepMandelbrotPerturbator> reuse(const CalculationSettings &calc, const double_exp &dcMax, int exp10);

    const DeepMandelbrotReference *getReference() const override;

    DeepMPATable &getTable() const override;

    double_exp getDcMaxExp() const override;
};

// DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR
// DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR
// DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR
// DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR
// DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR  DEFINITION OF DEEP MANDELBROT PERTURBATOR



inline const DeepMandelbrotReference *DeepMandelbrotPerturbator::getReference() const {
    return reference.get();
}

inline DeepMPATable &DeepMandelbrotPerturbator::getTable() const {
    return *table;
}

inline double_exp DeepMandelbrotPerturbator::getDcMaxExp() const {
    return dcMax;
}