//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include "../mrthy/MPATable.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtPerturbSettings.hpp"
#include "Perturbator.h"
#include "SeriesApproxImationData.hpp"

namespace merutilm::rff2 {


    struct MB2PerturbatorBase {

        ParallelRenderState &state;
        dex dcMax;
        complex<dex> off = complex<dex>::ZERO;

    protected:
        const FrtGeneralSettings &generalSettings;
        const FrtPerturbSettings &ptbSettings;
        const SeriesApproximationData &seriesApproximationData;

    public:
        MB2PerturbatorBase(ParallelRenderState &state, const dex dcMax, const FrtGeneralSettings &generalSettings,
                           const FrtPerturbSettings &ptbSettings, const SeriesApproximationData &seriesApproximationData) :
            state(state), dcMax(dcMax), generalSettings(generalSettings), ptbSettings(ptbSettings), seriesApproximationData(seriesApproximationData) {}

        virtual ~MB2PerturbatorBase() = default;

        [[nodiscard]] virtual double iterate(complex<dex> dci) const = 0;
    };

    template<Number Num>
    struct MB2Perturbator final : public Perturbator, public MB2PerturbatorBase {

        const MB2Reference<Num> &reference;
        const MPATable<Num> *table;

        explicit MB2Perturbator(ParallelRenderState &state, const dex dcMax, const FrtGeneralSettings &generalSettings,
                                const FrtPerturbSettings &ptbSettings, const SeriesApproximationData &seriesApproximationData, const MB2Reference<Num> &reference,
                                const MPATable<Num> *table) :
            MB2PerturbatorBase(state, dcMax, generalSettings, ptbSettings, seriesApproximationData), reference(reference), table(table) {}

        ~MB2Perturbator() override = default;


        [[nodiscard]] bool checkInteriorBasic(const complex<Num> c) const {
            if (calculatable::is_zero(c.im) && c.re < Num(0.25) && c.re >= Num(-2))
                return true;

            const double crm = static_cast<double>(c.re) - 0.25;
            const double cid = static_cast<double>(c.im);
            if (crm * crm + cid * cid < 0.5 * (-crm + std::sqrt(crm * crm + cid * cid)))
                return true;

            return false;
        }


        ;
        [[nodiscard]] double iterate(const complex<dex> dc) const override {
            if (state.interruptRequested())
                return 0.0;

            const complex<Num> dc0 = static_cast<complex<Num>>(dc + off);
#ifdef ENABLE_SERIES_APPROXIMATION
            // PROCESS SERIES APPROXIMATION (DEPRECATED)
            const complex dcSa = {dex{dc0.re}, dex{dc0.im}};
            complex dcSaM = dcSa;
            complex dzSa = complex<dex>::ZERO;

            for (const auto term: seriesApproximationData.terms) {
                dzSa += term * dcSaM;
                dcSaM *= dcSa;
            }

            uint64_t iteration = seriesApproximationData.skippedIterations;
            uint64_t refIteration = seriesApproximationData.skippedIterations;
            complex<Num> dz = {Num(dzSa.re), Num(dzSa.im)};
#else
            uint64_t iteration = 0;
            uint64_t refIteration = 0;
            complex<Num> dz = complex<Num>::ZERO;
#endif
            const uint64_t maxRefIteration = reference.longestPeriod();

            int absIteration = 0;
            Num currDistance2 = Num(0);
            Num prevDistance2 = currDistance2;
            const bool isAbs = ptbSettings.absoluteIterationMode;
            const uint64_t maxIteration = ptbSettings.maxIteration;
            const float bailout2 = generalSettings.bailout * generalSettings.bailout;

            complex<Num> c = reference.orbit(1) + dc0;

            if (checkInteriorBasic(c)) {
                return isAbs ? 1 : static_cast<double>(maxIteration);
            }


            while (iteration < maxIteration) {
                if (table != nullptr) {
                    if (PA<Num> *paPtr = table->lookup(refIteration, dz); paPtr != nullptr) {
                        PA<Num> &pa = *paPtr;

                        dz = pa.apply(dz, dc0);
                        iteration += pa.skip;
                        refIteration += pa.skip;
                        ++absIteration;

                        if (iteration >= maxIteration) {
                            return static_cast<double>(isAbs ? absIteration : maxIteration);
                        }
                    }
                }


                if (refIteration != maxRefIteration) {
                    const complex<Num> oldZ2 = reference.orbit(refIteration) * Num(2);
                    const complex<Num> oldPtbz = oldZ2 + dz;

                    dz = oldPtbz * dz + dc0;

                    ++refIteration;
                    ++iteration;
                    ++absIteration;
                }

                complex<Num> ptbz = reference.orbit(refIteration) + dz;


                prevDistance2 = currDistance2;
                currDistance2 = ptbz.norm_sqr();

                if (refIteration == maxRefIteration || currDistance2 < dz.norm_sqr()) {
                    refIteration = 0;
                    dz = ptbz;
                }

                dz = dz.try_normalized_value();

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
        }
        using LightMB2Perturbator = MB2Perturbator<double>;
        using DeepMB2Perturbator = MB2Perturbator<dex>;
    };
} // namespace merutilm::rff2
