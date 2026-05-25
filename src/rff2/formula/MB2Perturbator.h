//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "../mrthy/MPATable.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtPerturbSettings.hpp"
#include "Perturbator.h"

namespace merutilm::rff2 {


    struct MB2PerturbatorBase {

        ParallelRenderState &state;
        dex dcMax;
        dex offR = dex::ZERO;
        dex offI = dex::ZERO;

    protected:
        const FrtGeneralSettings &generalSettings;
        const FrtPerturbSettings &ptbSettings;

    public:
        MB2PerturbatorBase(ParallelRenderState &state, const dex dcMax, const FrtGeneralSettings &generalSettings,
                           const FrtPerturbSettings &ptbSettings) :
            state(state), dcMax(dcMax), generalSettings(generalSettings), ptbSettings(ptbSettings) {}

        virtual ~MB2PerturbatorBase() = default;

        [[nodiscard]] virtual double iterate(dex dcr, dex dci) const = 0;
    };

    template<Number Num>
    struct MB2Perturbator final : public Perturbator, public MB2PerturbatorBase {

        const MB2Reference<Num> &reference;
        const MPATable<Num> *table;

        explicit MB2Perturbator(ParallelRenderState &state, const dex dcMax, const FrtGeneralSettings &generalSettings,
                                const FrtPerturbSettings &ptbSettings, const MB2Reference<Num> &reference,
                                const MPATable<Num> *table) :
            MB2PerturbatorBase(state, dcMax, generalSettings, ptbSettings), reference(reference), table(table) {}

        ~MB2Perturbator() override = default;

        [[nodiscard]] double iterate(dex dcr, dex dci) const override {
            if (state.interruptRequested())
                return 0.0;

            const Num dcr1 = Num(offR.is_zero() ? dcr : dcr + offR);
            const Num dci1 = Num(offI.is_zero() ? dci : dci + offI);

            uint64_t iteration = 0;
            uint64_t refIteration = 0;
            int absIteration = 0;
            const uint64_t maxRefIteration = reference.longestPeriod();
            Num dzr = Num(0);
            Num dzi = Num(0);

            Num currDistance2 = Num(0);
            Num prevDistance2 = currDistance2;
            const bool isAbs = ptbSettings.absoluteIterationMode;
            const uint64_t maxIteration = ptbSettings.maxIteration;
            const float bailout2 = generalSettings.bailout * generalSettings.bailout;

            while (iteration < maxIteration) {
                if (table != nullptr) {
                    if (const PA<Num> *paPtr = table->lookup(refIteration, dzr, dzi); paPtr != nullptr) {
                        const PA<Num> &pa = *paPtr;
                        const Num dzr1 = pa.anr * dzr - pa.ani * dzi + pa.bnr * dcr1 - pa.bni * dci1;
                        const Num dzi1 = pa.anr * dzi + pa.ani * dzr + pa.bnr * dci1 + pa.bni * dcr1;

                        dzr = dzr1;
                        dzi = dzi1;
                        iteration += pa.skip;
                        refIteration += pa.skip;
                        ++absIteration;

                        if (iteration >= maxIteration) {
                            return static_cast<double>(isAbs ? absIteration : maxIteration);
                        }
                    }
                }


                if (refIteration != maxRefIteration) {
                    const uint64_t index = ArrayCompressor::compress(reference.compressor, refIteration);
                    const Num zr1 = index == 0 ? dzr : reference.refReal[index] * Num(2) + dzr;
                    const Num zi1 = index == 0 ? dzi : reference.refImag[index] * Num(2) + dzi;


                    if (calculatable::is_zero(dzr) && calculatable::is_zero(dzi)) {
                        dzr = dcr1;
                        dzi = dci1;
                    } else {
                        const Num zr2 = zr1 * dzr - zi1 * dzi + dcr1;
                        const Num zi2 = zr1 * dzi + zi1 * dzr + dci1;

                        dzr = zr2;
                        dzi = zi2;
                    }


                    ++refIteration;
                    ++iteration;
                    ++absIteration;
                }

                const uint64_t index = ArrayCompressor::compress(reference.compressor, refIteration);
                Num zr = reference.refReal[index] + dzr;
                Num zi = reference.refImag[index] + dzi;

                if (calculatable::is_zero(zi) && (calculatable::is_zero(zr) || (zr < Num(0.25) && zr >= Num(-2)))) {
                    // IT IS NOT SATISFIED MPA SKIP RADIUS CONDITION.
                    // WHEN THE MAX ITERATION IS HIGH, REPEATS SEMI-INFINITELY.
                    return static_cast<double>(maxIteration);
                }

                prevDistance2 = currDistance2;
                currDistance2 = zr * zr + zi * zi;


                if (refIteration == maxRefIteration || currDistance2 < dzr * dzr + dzi * dzi) {
                    refIteration = 0;
                    dzr = zr;
                    dzi = zi;
                }

                if constexpr (std::is_same_v<Num, dex>) {
                    dzr.try_normalize();
                    dzi.try_normalize();
                }

                if (static_cast<double>(currDistance2) > bailout2)
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

            const double prevDistance = sqrt(static_cast<double>(prevDistance2));
            const double currDistance = sqrt(static_cast<double>(currDistance2));

            return FrtDecimalizeIterationMethodUtil::getDoubleValueIteration(iteration, prevDistance, currDistance,
                                                                             ptbSettings.decimalizeIterationMethod,
                                                                             generalSettings.bailout);
        };
        using LightMB2Perturbator = MB2Perturbator<double>;
        using DeepMB2Perturbator = MB2Perturbator<dex>;
    };
} // namespace merutilm::rff2
