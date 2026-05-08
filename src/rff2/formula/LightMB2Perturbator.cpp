//
// Created by Merutilm on 2025-05-10.
//

#include "LightMB2Perturbator.h"

#include <cmath>


#include "Perturbator.h"


namespace merutilm::rff2 {


    LightMB2Perturbator::LightMB2Perturbator(
            ParallelRenderState &state, const FractalSettings &calc, const double dcMax, const int exp10,
            const uint64_t refInitialCapacity, const uint64_t fixedPeriod, ApproxTableManager &tableRef,
            std::function<void(uint64_t)> &&actionPerRefCalcIteration,
            std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration, const bool arbitraryPrecisionFPGBn,
            std::unique_ptr<LightMB2Reference> reusedReference, std::unique_ptr<LightMPATable> reusedTable,
            const double offR, const double offI) :
        MB2Perturbator(state, calc), dcMax(dcMax), offR(offR), offI(offI) {

        if (reusedReference == nullptr) {
            this->referenceCreationResult = LightMB2Reference::generateReference(state, calc, exp10, refInitialCapacity, fixedPeriod, dcMax,
                                                                  arbitraryPrecisionFPGBn,
                                                                  std::move(actionPerRefCalcIteration), &reference);

        } else {
            reference = std::move(reusedReference);
            this->referenceCreationResult = Reference::CreationResult::SUCCESS;
        }

        if (this->referenceCreationResult != Reference::CreationResult::SUCCESS) {
            return;
        }

        if (reusedTable == nullptr) {
            table = std::make_unique<LightMPATable>(state, *reference, &calc.mpaSettings, dcMax, tableRef,
                                                    std::move(actionPerCreatingTableIteration));
        } else {
            table = std::move(reusedTable);
        }
    }


    double LightMB2Perturbator::iterate(const dex dcr, const dex dci) const {
        if (state.interruptRequested())
            return 0.0;

        const double dcr1 = static_cast<double>(dcr) + offR;
        const double dci1 = static_cast<double>(dci) + offI;

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
        const float bailout2 = bailout * bailout;


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


            if (zi == 0 && zr < 0.25 && zr >= -2) {
                // IT IS NOT SATISFIED MPA SKIP RADIUS CONDITION.
                // WHEN THE MAX ITERATION IS HIGH, REPEATS SEMI-INFINITELY.
                return static_cast<double>(maxIteration);
            }

            pd = cd;
            cd = zr * zr + zi * zi;

            if (refIteration == maxRefIteration || cd < dzr * dzr + dzi * dzi) {
                refIteration = 0;
                dzr = zr;
                dzi = zi;
            }


            if (cd > bailout2) {
                break;
            }

            if (absIteration % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested())
                return 0.0;
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


    std::unique_ptr<LightMB2Perturbator>
    LightMB2Perturbator::reuse(const FractalSettings &calc, const double dcMax, ApproxTableManager &tableRef) {

        const int exp10 = logZoomToExp10(calc.logZoom);
        double offR = 0;
        double offI = 0;
        uint64_t longestPeriod = 1;
        std::unique_ptr<LightMB2Reference> reusedReference = nullptr;

        if (this->referenceCreationResult != Reference::CreationResult::SUCCESS) {
            // try to use incomplete reference
            MessageBox(nullptr, "Please do not try to use incomplete Reference.", "Warning",
                       MB_OK | MB_ICONWARNING);
        } else {
            fixed_point_complex_i1 center = calc.center.create_variant(exp10);
            const fixed_point_complex_i1 refCenter = reference->center.create_variant(exp10);

            fixed_point_complex::sub(center, center, refCenter);

            offR = center.get_real().double_value();
            offI = center.get_imag().double_value();
            longestPeriod = reference->longestPeriod();
            reusedReference = std::move(reference);
        }


        return std::make_unique<LightMB2Perturbator>(
                state, calc, dcMax, exp10, 0, longestPeriod, tableRef,
                [](uint64_t) {
                    // no action because the reference is already declared
                },
                [](uint64_t, double) {
                    // same reason
                },
                false, std::move(reusedReference), std::move(table), offR, offI);
    }
} // namespace merutilm::rff2
