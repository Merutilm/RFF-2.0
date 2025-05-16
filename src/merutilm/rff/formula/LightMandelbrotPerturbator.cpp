//
// Created by Merutilm on 2025-05-10.
//

#include "LightMandelbrotPerturbator.h"

#include <cmath>
#include <iostream>

#include "Perturbator.h"


LightMandelbrotPerturbator::LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                       const double dcMax, const int exp10,
                                                       const uint64_t initialPeriod,
                                                       std::vector<std::vector<LightPA> > &&previousAllocatedTable,
                                                       std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                       std::function<void(uint64_t, double)> &&
                                                       actionPerCreatingTableIteration) : LightMandelbrotPerturbator(
    state, calc, dcMax, exp10, initialPeriod, std::move(previousAllocatedTable), std::move(actionPerRefCalcIteration),
    std::move(actionPerCreatingTableIteration), false) {
}

LightMandelbrotPerturbator::LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                       const double dcMax, const int exp10,
                                                       const uint64_t initialPeriod,
                                                       std::vector<std::vector<LightPA> > &&previousAllocatedTable,
                                                       std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                       std::function<void(uint64_t, double)> &&
                                                       actionPerCreatingTableIteration,
                                                       const bool arbitraryPrecisionFPGBn) : LightMandelbrotPerturbator(
    state, calc, dcMax, exp10, initialPeriod, std::move(previousAllocatedTable), std::move(actionPerRefCalcIteration),
    std::move(actionPerCreatingTableIteration), arbitraryPrecisionFPGBn, nullptr, nullptr, 0, 0) {
}

LightMandelbrotPerturbator::LightMandelbrotPerturbator(ParallelRenderState &state, const CalculationSettings &calc,
                                                       const double dcMax, const int exp10,
                                                       const uint64_t initialPeriod,
                                                       std::vector<std::vector<LightPA> > &&previousAllocatedTable,
                                                       std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                       std::function<void(uint64_t, double)> &&
                                                       actionPerCreatingTableIteration,
                                                       const bool arbitraryPrecisionFPGBn,
                                                       std::unique_ptr<LightMandelbrotReference> reusedReference,
                                                       std::unique_ptr<LightMPATable> reusedTable,
                                                       const double offR,
                                                       const double offI) : state(state), calc(calc), dcMax(dcMax), offR(offR), offI(offI),
                                                                            arbitraryPrecisionFPGBn(
                                                                                arbitraryPrecisionFPGBn) {
    if (reusedReference == nullptr) {
        reference = LightMandelbrotReference::createReference(state, calc, exp10, initialPeriod, dcMax,
                                                              arbitraryPrecisionFPGBn,
                                                              std::move(actionPerRefCalcIteration));
    } else {
        reference = std::move(reusedReference);
    }

    if (reusedTable == nullptr) {
        table = std::make_unique<LightMPATable>(state, reference.get(), &calc.mpaSettings, dcMax,
                                                std::move(previousAllocatedTable),
                                                std::move(actionPerCreatingTableIteration));
    } else {
        table = std::move(reusedTable);
    }
}


double LightMandelbrotPerturbator::iterate(const double dcr, const double dci) const {
    if (state.interruptRequested()) return 0.0;

    const double dcr1 = dcr + offR;
    const double dci1 = dci + offI;

    uint64_t iteration = 0;
    uint64_t refIteration = 0;
    int absIteration = 0;
    const uint64_t maxRefIteration = reference->longestPeriod();

    double dzr = 0; // delta z
    double dzi = 0;
    double zr = 0; // z
    double zi = 0;

    double cd = 0;
    double pd = cd;
    const bool isAbs = calc.absoluteIterationMode;
    const uint64_t maxIteration = calc.maxIteration;
    const float bailout = calc.bailout;


    while (iteration < maxIteration) {
        if (table != nullptr) {
            if (const LightPA *mpaPtr = table->lookup(refIteration, dzr, dzi); mpaPtr != nullptr) {
                const LightPA &mpa = *mpaPtr;
                const double dzr1 = mpa.anr * dzr - mpa.ani * dzi + mpa.bnr * dcr1 - mpa.bni * dci1;
                const double dzi1 = mpa.anr * dzi + mpa.ani * dzr + mpa.bnr * dci1 + mpa.bni * dcr1;

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
            const double zr1 = reference->refReal[index] * 2 + dzr;
            const double zi1 = reference->refImag[index] * 2 + dzi;

            const double zr2 = zr1 * dzr - zi1 * dzi + dcr1;
            const double zi2 = zr1 * dzi + zi1 * dzr + dci1;

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
        cd = zr * zr + zi * zi;

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

    return Perturbator::getDoubleValueIteration(iteration, pd, cd, calc.decimalizeIterationMethod, bailout);
}


std::unique_ptr<LightMandelbrotPerturbator> LightMandelbrotPerturbator::reuse(
    const CalculationSettings &calc, const double dcMax,
    const int exp10) {
    double offR = 0;
    double offI = 0;
    uint64_t longestPeriod = 1;
    std::unique_ptr<LightMandelbrotReference> reusedReference = nullptr;

    if (reference == RFF::NullPointer::PROCESS_TERMINATED_REFERENCE) {
        //try to use process-terminated reference
        MessageBox(nullptr, "Please do not try to use PROCESS-TERMINATED Reference.", "Warning",
                   MB_OK | MB_ICONWARNING);
    } else {
        GMPComplexCalculator centerOffset = calc.center.edit(exp10);
        centerOffset -= reference->center.edit(exp10);
        offR = centerOffset.getReal().doubleValue();
        offI = centerOffset.getImag().doubleValue();
        longestPeriod = reference->longestPeriod();
        reusedReference = std::move(reference);
    }


    return std::make_unique<LightMandelbrotPerturbator>(state, calc, dcMax, exp10, longestPeriod,
                                                        std::vector<std::vector<LightPA> >(),
                                                        [](uint64_t) {
                                                            //no action because the reference is already declared
                                                        }, [](uint64_t, double) {
                                                            //same reason
                                                        }, arbitraryPrecisionFPGBn, std::move(reusedReference),
                                                        std::move(table), offR, offI);
}
