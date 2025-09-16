//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "DeepMandelbrotReference.h"
#include "MandelbrotPerturbator.h"
#include "../mrthy/DeepMPATable.h"

namespace merutilm::rff2 {
    class DeepMandelbrotPerturbator final : public MandelbrotPerturbator {
        std::unique_ptr<DeepMandelbrotReference> reference = nullptr;
        std::unique_ptr<DeepMPATable> table = nullptr;

        const dex dcMax;
        const dex offR;
        const dex offI;

    public:

        explicit DeepMandelbrotPerturbator(ParallelRenderState &state, const FractalAttribute &calc,
                                           const dex &dcMax, int exp10,
                                           uint64_t initialPeriod, ApproxTableCache &tableRef,
                                           std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                           std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration,
                                           bool arbitraryPrecisionFPGBn = false,
                                           std::unique_ptr<DeepMandelbrotReference> reusedReference = nullptr,
                                           std::unique_ptr<DeepMPATable> reusedTable = nullptr,
                                           const dex &offR = dex::ZERO, const dex &offI = dex::ZERO);


        [[nodiscard]] double iterate(const dex &dcr, const dex &dci) const override;

        std::unique_ptr<DeepMandelbrotPerturbator> reuse(const FractalAttribute &calc, const dex &dcMax,
                                                         ApproxTableCache &tableRef);

        [[nodiscard]] const DeepMandelbrotReference *getReference() const override;

        [[nodiscard]] DeepMPATable &getTable() const;

        [[nodiscard]] dex getDcMaxAsDoubleExp() const override;
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

    inline dex DeepMandelbrotPerturbator::getDcMaxAsDoubleExp() const {
        return dcMax;
    }
}
