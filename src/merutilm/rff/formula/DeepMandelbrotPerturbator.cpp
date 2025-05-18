//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMandelbrotPerturbator.h"

#include <iostream>

#include "../calc/double_exp_math.h"

DeepMandelbrotPerturbator::DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                     const double_exp &dcMax, const int exp10,
                                                     const uint64_t initialPeriod,
                                                     std::vector<std::vector<DeepPA> > &&previousAllocatedTable,
                                                     std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                     std::function<void(uint64_t, double)> &&
                                                     actionPerCreatingTableIteration) : DeepMandelbrotPerturbator(
    state, calc, dcMax, exp10, initialPeriod, std::move(previousAllocatedTable), std::move(actionPerRefCalcIteration),
    std::move(actionPerCreatingTableIteration), false) {
}

DeepMandelbrotPerturbator::DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                       const double_exp &dcMax, const int exp10,
                                                       const uint64_t initialPeriod,
                                                       std::vector<std::vector<DeepPA> > &&previousAllocatedTable,
                                                       std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                       std::function<void(uint64_t, double)> &&
                                                       actionPerCreatingTableIteration,
                                                       const bool arbitraryPrecisionFPGBn) : DeepMandelbrotPerturbator(
    state, calc, dcMax, exp10, initialPeriod, std::move(previousAllocatedTable), std::move(actionPerRefCalcIteration),
    std::move(actionPerCreatingTableIteration), arbitraryPrecisionFPGBn, nullptr, nullptr, RFF::Precision::DEX_ZERO, RFF::Precision::DEX_ZERO) {
}

DeepMandelbrotPerturbator::DeepMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                       const double_exp &dcMax, const int exp10,
                                                       const uint64_t initialPeriod,
                                                       std::vector<std::vector<DeepPA> > &&previousAllocatedTable,
                                                       std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                       std::function<void(uint64_t, double)> &&
                                                       actionPerCreatingTableIteration,
                                                       const bool arbitraryPrecisionFPGBn,
                                                       std::unique_ptr<DeepMandelbrotReference> reusedReference,
                                                       std::unique_ptr<DeepMPATable> reusedTable,
                                                       const double_exp &offR,
                                                       const double_exp &offI) : MandelbrotPerturbator(state, calc), dcMax(dcMax), offR(offR), offI(offI){
    if (reusedReference == nullptr) {
        reference = DeepMandelbrotReference::createReference(state, calc, exp10, initialPeriod, dcMax,
                                                              arbitraryPrecisionFPGBn,
                                                              std::move(actionPerRefCalcIteration));
    } else {
        reference = std::move(reusedReference);
    }

    if (reusedTable == nullptr) {
        table = std::make_unique<DeepMPATable>(state, reference.get(), &calc.mpaSettings, dcMax,
                                                std::move(previousAllocatedTable),
                                                std::move(actionPerCreatingTableIteration));
    } else {
        table = std::move(reusedTable);
    }
}


double DeepMandelbrotPerturbator::iterate(const double_exp &dcr, const double_exp &dci) const {
    if (state.interruptRequested()) return 0.0;

    const double_exp dcr1 = dcr + offR;
    const double_exp dci1 = dci + offI;

    uint64_t iteration = 0;
    uint64_t refIteration = 0;
    int absIteration = 0;
    const uint64_t maxRefIteration = reference->longestPeriod();

    double_exp dzr = RFF::Precision::DEX_ZERO;
    double_exp dzi = RFF::Precision::DEX_ZERO;
    double_exp zr = RFF::Precision::DEX_ZERO;
    double_exp zi = RFF::Precision::DEX_ZERO;

    double cd = 0;
    double pd = cd;
    const bool isAbs = calc.absoluteIterationMode;
    const uint64_t maxIteration = calc.maxIteration;
    const float bailout = calc.bailout;


    while (iteration < maxIteration) {
        if (table != nullptr) {
            if (const DeepPA *mpaPtr = table->lookup(refIteration, dzr, dzi); mpaPtr != nullptr) {
                const DeepPA &mpa = *mpaPtr;
                const double_exp dzr1 = mpa.anr * dzr - mpa.ani * dzi + mpa.bnr * dcr1 - mpa.bni * dci1;
                const double_exp dzi1 = mpa.anr * dzi + mpa.ani * dzr + mpa.bnr * dci1 + mpa.bni * dcr1;

                dzr = dzr1;
                dzi = dzi1;

                iteration += mpa.skip;
                refIteration += mpa.skip;
                ++absIteration;

                if (iteration >= maxIteration) {
                    return static_cast<double>(isAbs ? absIteration : maxIteration);
                }
                continue;
            }
        }


        if (refIteration != maxRefIteration) {
            const uint64_t index = ArrayCompressor::compress(reference->compressor, refIteration);
            const double_exp zr1 = reference->refReal[index] * 2 + dzr;
            const double_exp zi1 = reference->refImag[index] * 2 + dzi;

            const double_exp zr2 = zr1 * dzr - zi1 * dzi + dcr1;
            const double_exp zi2 = zr1 * dzi + zi1 * dzr + dci1;

            dzr = zr2;
            dzi = zi2;


            ++refIteration;
            ++iteration;
            ++absIteration;
        }

        const uint64_t index = ArrayCompressor::compress(reference->compressor, refIteration);
        zr = reference->refReal[index] + dzr;
        zi = reference->refImag[index] + dzi;


        pd = cd;
        cd = static_cast<double>(dex_trigonometric::hypot2(zr, zi));

        if (refIteration == maxRefIteration || cd < dzr * dzr + dzi * dzi) {
            refIteration = 0;
            dzr = zr;
            dzi = zi;
        }


        if (cd > bailout * bailout) {
            break;
        }

        if (absIteration % RFF::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) return 0.0;
    }

    if (isAbs) {
        return absIteration;
    }

    if (iteration >= maxIteration) {
        return static_cast<double>(maxIteration);
    }

    pd = sqrt(pd);
    cd = sqrt(cd);

    return getDoubleValueIteration(iteration, pd, cd, calc.decimalizeIterationMethod, bailout);
}


std::unique_ptr<DeepMandelbrotPerturbator> DeepMandelbrotPerturbator::reuse(
    const CalculationSettings &calc, const double_exp &dcMax,
    const int exp10) {
    double_exp offR = RFF::Precision::DEX_ZERO;
    double_exp offI = RFF::Precision::DEX_ZERO;
    uint64_t longestPeriod = 1;
    std::unique_ptr<DeepMandelbrotReference> reusedReference = nullptr;

    if (reference == RFF::NullPointer::PROCESS_TERMINATED_REFERENCE) {
        //try to use process-terminated reference
        MessageBox(nullptr, "Please do not try to use PROCESS-TERMINATED Reference.", "Warning",
                   MB_OK | MB_ICONWARNING);
    } else {
        fp_complex_calculator centerOffset = calc.center.edit(exp10);
        centerOffset -= reference->center.edit(exp10);
        offR = centerOffset.getReal().double_exp_value();
        offI = centerOffset.getImag().double_exp_value();
        longestPeriod = reference->longestPeriod();
        reusedReference = std::move(reference);
    }


    return std::make_unique<DeepMandelbrotPerturbator>(state, calc, dcMax, exp10, longestPeriod,
                                                        std::vector<std::vector<DeepPA> >(),
                                                        [](uint64_t) {
                                                            //no action because the reference is already declared
                                                        }, [](uint64_t, double) {
                                                            //same reason
                                                        }, false, std::move(reusedReference),
                                                        std::move(table), offR, offI);
}
