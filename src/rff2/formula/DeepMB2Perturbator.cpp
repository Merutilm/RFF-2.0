//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMB2Perturbator.h"


namespace merutilm::rff2 {


    double DeepMB2Perturbator::iterate(const dex dcr, const dex dci) const {
        if (state.interruptRequested())
            return 0.0;

        const dex dcr1 = offR.is_zero() ? dcr : dcr + offR;
        const dex dci1 = offI.is_zero() ? dci : dci + offI;

        uint64_t iteration = 0;
        uint64_t refIteration = 0;
        int absIteration = 0;
        const uint64_t maxRefIteration = reference.longestPeriod();
        dex dzr = dex::ZERO;
        dex dzi = dex::ZERO;


        constexpr auto two = dex(2);
        constexpr auto zrMin = dex(-2);
        constexpr auto zrMax = dex(0.25);

        double cd = 0;
        double pd = cd;
        const bool isAbs = ptbSettings.absoluteIterationMode;
        const uint64_t maxIteration = ptbSettings.maxIteration;
        const float bailout2 = generalSettings.bailout * generalSettings.bailout;

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
                const uint64_t index = ArrayCompressor::compress(reference.compressor, refIteration);
                const dex zr1 = index == 0 ? dzr : reference.refReal[index] * two + dzr;
                const dex zi1 = index == 0 ? dzi : reference.refImag[index] * two + dzi;


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

            const uint64_t index = ArrayCompressor::compress(reference.compressor, refIteration);
            dex zr = reference.refReal[index] + dzr;
            dex zi = reference.refImag[index] + dzi;

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

        return FrtDecimalizeIterationMethodUtil::getDoubleValueIteration(iteration, fpd, fcd, ptbSettings.decimalizeIterationMethod, generalSettings.bailout);
    }
} // namespace merutilm::rff2
