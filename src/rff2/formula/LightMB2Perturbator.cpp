//
// Created by Merutilm on 2025-05-10.
//

#include "LightMB2Perturbator.h"

#include <cmath>


#include "Perturbator.h"


namespace merutilm::rff2 {


    double LightMB2Perturbator::iterate(const dex dcr, const dex dci) const {
        if (state.interruptRequested())
            return 0.0;

        const double dcr1 = static_cast<double>(dcr) + static_cast<double>(offR);
        const double dci1 = static_cast<double>(dci) + static_cast<double>(offI);

        uint64_t iteration = 0;
        uint64_t refIteration = 0;
        int absIteration = 0;
        const uint64_t maxRefIteration = reference.longestPeriod();

        double dzr = 0; // delta z
        double dzi = 0;
        double zr = 0; // z
        double zi = 0;

        double cd = 0;
        double pd = cd;
        const bool isAbs = ptbSettings.absoluteIterationMode;
        const uint64_t maxIteration = ptbSettings.maxIteration;
        const float bailout = generalSettings.bailout;
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
                const uint64_t index = ArrayCompressor::compress(reference.compressor, refIteration);
                const double zr1 = reference.refReal[index] * 2 + dzr;
                const double zi1 = reference.refImag[index] * 2 + dzi;

                const double zr2 = zr1 * dzr - zi1 * dzi + dcr1;
                const double zi2 = zr1 * dzi + zi1 * dzr + dci1;

                dzr = zr2;
                dzi = zi2;


                ++refIteration;
                ++iteration;
                ++absIteration;
            }

            const uint64_t index = ArrayCompressor::compress(reference.compressor, refIteration);
            zr = reference.refReal[index] + dzr;
            zi = reference.refImag[index] + dzi;


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

        return FrtDecimalizeIterationMethodUtil::getDoubleValueIteration(iteration, pd, cd, ptbSettings.decimalizeIterationMethod, bailout);
    }
} // namespace merutilm::rff2
