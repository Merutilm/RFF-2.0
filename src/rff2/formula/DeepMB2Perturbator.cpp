//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMB2Perturbator.h"

namespace merutilm::rff2 {


    DeepMB2Perturbator::DeepMB2Perturbator(
            ParallelRenderState &state, const FractalSettings &calc, const dex dcMax, const int exp10,
            const uint64_t refInitialCapacity, const uint64_t fixedPeriod,
            std::function<void(uint64_t)> &&actionPerRefCalcIteration,
            std::function<void(uint64_t, double)> &&actionPerCreatingTableIteration, const bool arbitraryPrecisionFPGBn,
            std::unique_ptr<DeepMB2Reference> reusedReference, std::unique_ptr<DeepMPATable> reusedTable,
            const dex offR, const dex offI) :
        MB2Perturbator(state, calc), dcMax(dcMax), offR(offR), offI(offI) {

        if (reusedReference == nullptr) {
            this->referenceCreationResult = DeepMB2Reference::createReference(
                    state, calc, exp10, refInitialCapacity, fixedPeriod, dcMax, arbitraryPrecisionFPGBn,
                    std::move(actionPerRefCalcIteration), &reference);
        } else {
            reference = std::move(reusedReference);
            this->referenceCreationResult = Reference::CreationResult::SUCCESS;
        }

        if (this->referenceCreationResult != Reference::CreationResult::SUCCESS) {
            reference = nullptr;
            return;
        }

        if (reusedTable == nullptr) {
            table = std::make_unique<DeepMPATable>(state, *reference, &calc.mpaSettings, dcMax,
                                                   std::move(actionPerCreatingTableIteration));
        } else {
            table = std::move(reusedTable);
        }
    }


    double DeepMB2Perturbator::iterate(const FractalSettings &calc, const dex dcr, const dex dci) const {
        if (state.interruptRequested())
            return 0.0;

        const dex dcr1 = offR.is_zero() ? dcr : dcr + offR;
        const dex dci1 = offI.is_zero() ? dci : dci + offI;

        uint64_t iteration = 0;
        uint64_t refIteration = 0;
        int absIteration = 0;
        const uint64_t maxRefIteration = reference->longestPeriod();
        dex dzr = dex::ZERO;
        dex dzi = dex::ZERO;


        constexpr auto two = dex(2);
        constexpr auto zrMin = dex(-2);
        constexpr auto zrMax = dex(0.25);

        double cd = 0;
        double pd = cd;
        const bool isAbs = calc.absoluteIterationMode;
        const uint64_t maxIteration = calc.maxIteration;
        const float bailout = calc.bailout;
        const float bailout2 = bailout * bailout;

        while (iteration < maxIteration) {
            if (table != nullptr) {
                if (const DeepPA *mpaPtr = table->lookup(refIteration, dzr, dzi); mpaPtr != nullptr) {
                    const DeepPA &mpa = *mpaPtr;
                    const dex dzr1 = mpa.anr * dzr - mpa.ani * dzi + mpa.bnr * dcr1 - mpa.bni * dci1;
                    const dex dzi1 = mpa.anr * dzi + mpa.ani * dzr + mpa.bnr * dci1 + mpa.bni * dcr1;

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
                const dex zr1 = index == 0 ? dzr : reference->refReal[index] * two + dzr;
                const dex zi1 = index == 0 ? dzi : reference->refImag[index] * two + dzi;


                if (dzr.is_zero() && dzi.is_zero()) {
                    dzr = dcr1;
                    dzi = dci1;
                } else {
                    const dex zr2 = zr1 * dzr - zi1 * dzi + dcr1;
                    const dex zi2 = zr1 * dzi + zi1 * dzr + dci1;

                    dzr = zr2;
                    dzi = zi2;
                }


                ++refIteration;
                ++iteration;
                ++absIteration;
            }

            const uint64_t index = ArrayCompressor::compress(reference->compressor, refIteration);
            dex zr = reference->refReal[index] + dzr;
            dex zi = reference->refImag[index] + dzi;

            if (zi.is_zero() && (zr.is_zero() || (zr < zrMax && zr >= zrMin))) {
                // IT IS NOT SATISFIED MPA SKIP RADIUS CONDITION.
                // WHEN THE MAX ITERATION IS HIGH, REPEATS SEMI-INFINITELY.
                return static_cast<double>(maxIteration);
            }

            const auto zr0 = static_cast<double>(zr);
            const auto zi0 = static_cast<double>(zi);
            const auto dzr0 = static_cast<double>(dzr);
            const auto dzi0 = static_cast<double>(dzi);

            pd = cd;
            cd = zr0 * zr0 + zi0 * zi0;

            if (refIteration == maxRefIteration || cd < dzr0 * dzr0 + dzi0 * dzi0) {
                refIteration = 0;
                dzr = zr;
                dzi = zi;
            }

            dzr.try_normalize();
            dzi.try_normalize();
            if (cd > bailout2)
                break;
            if (absIteration % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested())
                return 0.0;
        }

        if (isAbs) {
            return absIteration;
        }

        if (iteration >= maxIteration) {
            return static_cast<double>(maxIteration);
        }

        const double fpd = sqrt(pd);
        const double fcd = sqrt(cd);

        return FrtDecimalizeIterationMethodUtil::getDoubleValueIteration(iteration, fpd, fcd, calc.decimalizeIterationMethod, bailout);
    }


    std::unique_ptr<DeepMB2Perturbator>
    DeepMB2Perturbator::reuse(const FractalSettings &calc, const dex dcMax) {
        dex offR = dex::ZERO;
        dex offI = dex::ZERO;
        uint64_t longestPeriod = 1;
        std::unique_ptr<DeepMB2Reference> reusedReference = nullptr;

        const int exp10 = logZoomToExp10(calc.logZoom);

        if (this->referenceCreationResult != Reference::CreationResult::SUCCESS) {
            // try to use incomplete reference
            MessageBox(nullptr, "Please do not try to use incomplete Reference.", "Warning", MB_OK | MB_ICONWARNING);
        } else {

            fixed_point_complex_i1 center = calc.center.create_variant(exp10);
            const fixed_point_complex_i1 refCenter = reference->center.create_variant(exp10);
            fixed_point_complex_i1::sub(center, center, refCenter);

            offR = center.get_real().dex_value();
            offI = center.get_imag().dex_value();
            longestPeriod = reference->longestPeriod();
            reusedReference = std::move(reference);
        }


        return std::make_unique<DeepMB2Perturbator>(
                state, calc, dcMax, exp10, 0, longestPeriod,
                [](uint64_t) {
                    // no action because the reference is already declared
                },
                [](uint64_t, double) {
                    // same reason
                },
                false, std::move(reusedReference), std::move(table), offR, offI);
    }
} // namespace merutilm::rff2
