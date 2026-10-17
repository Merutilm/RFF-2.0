//
// Created by Merutilm on 2025-05-18.
//

#pragma once

#include "../mrthy/MPATable.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtPerturbSettings.hpp"
#include "../settings/FrtSASettings.hpp"
#include "Perturbator.h"
#include "SeriesApproximationData.hpp"

#include "../parallel/SIMDChunk.hpp"
#include "../parallel/SIMDChunkData.hpp"

namespace merutilm::rff2 {


    struct MB2PerturbatorBase {

        ParallelRenderState &state;
        dex dcMax;
        complex<dex> off = complex<dex>::ZERO;

    protected:
        const FrtGeneralSettings &generalSettings;
        const FrtSASettings &saSettings;
        const FrtPerturbSettings &ptbSettings;
        const SeriesApproximationData &seriesApproximationData;

    public:
        MB2PerturbatorBase(ParallelRenderState &state, const dex dcMax, const FrtGeneralSettings &generalSettings,
                           const FrtSASettings &saSettings, const FrtPerturbSettings &ptbSettings,
                           const SeriesApproximationData &seriesApproximationData) :
            state(state), dcMax(dcMax), generalSettings(generalSettings), saSettings(saSettings),
            ptbSettings(ptbSettings), seriesApproximationData(seriesApproximationData) {}

        virtual ~MB2PerturbatorBase() = default;

        [[nodiscard]] virtual double iterate(const complex<dex> &dc) const noexcept = 0;

        virtual void iterateBatch(SIMDChunk<double> &chunk,
                                  std::array<complex<dex>, RFF_SIMD_CHUNK_SIZE> &dcs) const noexcept = 0;
    };

    template<Number Num>
    struct MB2Perturbator final : public Perturbator, public MB2PerturbatorBase {

        static constexpr uint32_t PERIODIC_POINT_HISTORY_LENGTH = 8;
        const MB2Reference<Num> &reference;
        const MPATable<Num> &table;

        explicit MB2Perturbator(ParallelRenderState &state, const dex dcMax, const FrtGeneralSettings &generalSettings,
                                const FrtSASettings &saSettings, const FrtPerturbSettings &ptbSettings,
                                const SeriesApproximationData &seriesApproximationData,
                                const MB2Reference<Num> &reference, const MPATable<Num> &table) :
            MB2PerturbatorBase(state, dcMax, generalSettings, saSettings, ptbSettings, seriesApproximationData),
            reference(reference), table(table) {}

        ~MB2Perturbator() override = default;


        [[nodiscard]] bool checkInteriorBasic(const complex<Num> c) const {
            if (rff_math::is_zero(c.im) && c.re < Num(0.25) && c.re >= Num(-2))
                return true;

            const auto cr = static_cast<double>(c.re);
            const auto ci = static_cast<double>(c.im);

            // fast calculation of main bulb
            if (const auto crm025 = cr - 0.25;
                crm025 * crm025 + ci * ci < 0.5 * (-crm025 + std::sqrt(crm025 * crm025 + ci * ci)))
                return true;

            if (const auto crp100 = cr + 1; crp100 * crp100 + ci * ci < 0.0625)
                return true;


            return false;
        }

        bool checkInteriorAndApply(const complex<Num> &dz, const Num dcMax0, const float interiorDetectRadius,
                           std::array<complex<Num>, PERIODIC_POINT_HISTORY_LENGTH> &pdz,
                           int &pdzIndex) const {
            if (interiorDetectRadius == 0) return false;

            for (const complex<Num> &pdz0: pdz) {
                Num rad = (pdz0 - dz).norm_approx();
                if (rad * rad < dcMax0 * interiorDetectRadius) {
                    return true;
                }
            }

            pdz[pdzIndex++] = dz;
            if (pdzIndex == pdz.size())
                pdzIndex = 0;

            return false;
        }
        void applyMPApproximation(complex<Num> &dz, const complex<Num> &dc0, uint64_t &iteration, uint64_t &refIteration) const {
            if (const PA<Num> *paPtr = table.lookup(refIteration, dz); paPtr != nullptr) {
                const PA<Num> &pa = *paPtr;

                dz = pa.apply(dz, dc0);
                iteration += pa.skip;
                refIteration += pa.skip;
            }
        }

        void applySeriesApproximation(complex<Num> &dz, const complex<Num> &dc0, uint64_t &iteration, uint64_t &refIteration) const {
            const complex dcSa = {dex{dc0.re}, dex{dc0.im}};
            complex dcSaM = dcSa;
            complex dzSa = complex<dex>::ZERO;

            for (const auto term: seriesApproximationData.terms) {
                dzSa = (dzSa + term * dcSaM).try_normalized_value();
                dcSaM = (dcSaM * dcSa).try_normalized_value();
            }

            iteration = seriesApproximationData.skippedIterations;
            refIteration = seriesApproximationData.skippedIterations;
            dz = {Num(dzSa.re), Num(dzSa.im)};
        }

        static void updateDistance(const complex<Num> &z, double &prevDistance2, double &currDistance2) {
            prevDistance2 = currDistance2;
            currDistance2 = static_cast<complex<double>>(z).norm_sqr();
        }

        static bool tryRebase(const complex<Num> &z, complex<Num> &dz, uint64_t &refIteration, const uint64_t maxRefIteration) {
            if (refIteration == maxRefIteration || z.norm_approx() < dz.norm_approx()) {
                refIteration = 0;
                dz = z;
                return true;
            }
            return false;
        }

        complex<Num> perturbationStep(complex<Num> &dz, const complex<Num> &dc0, uint64_t &iteration, uint64_t &refIteration) const {
            dz = (reference.orbit(refIteration) * 2 + dz) * dz + dc0;
            const complex<Num> z = reference.orbit(refIteration + 1) + dz;

            ++iteration;
            ++refIteration;
            return z;
        }

        void calcInitialIteration(complex<Num> &dz, const complex<Num> &dc0, uint64_t &iteration, uint64_t &refIteration) const {
            if (saSettings.use) {
                applySeriesApproximation(dz, dc0, iteration, refIteration);
            } else {
                dz = complex<Num>::ZERO;
                iteration = 0;
                refIteration = 0;
            }
        }
        double iterate(const complex<dex> &dc) const noexcept override {
            if (state.interruptRequested())
                return 0.0;

            // field reference
            const auto dc0 = static_cast<complex<Num>>(dc + off);
            const Num dcMax0 = Num(dcMax);
            const bool isAbs = ptbSettings.absoluteIterationMode;
            const uint64_t maxIteration = ptbSettings.maxIteration;
            const uint64_t maxRefIteration = reference.longestPeriod();
            const float interiorDetectRadius = ptbSettings.interiorDetectRadiusPower == 0 ? 0 : pow(10, -ptbSettings.interiorDetectRadiusPower);
            const double bailout2 = generalSettings.bailout * generalSettings.bailout;

            // basic interior detection
            complex<Num> c = reference.orbit(1) + dc0;
            if (checkInteriorBasic(c)) {
                return isAbs ? 1 : static_cast<double>(maxIteration);
            }

            // generate initial iteration and dz
            complex<Num> dz;
            uint64_t iteration;
            uint64_t refIteration;
            calcInitialIteration(dz, dc0, iteration, refIteration);

            // init local variables
            std::array<complex<Num>, PERIODIC_POINT_HISTORY_LENGTH> pdz = {};
            int pdzIndex = 0;
            double currDistance2 = 0;
            double prevDistance2 = 0;

            // predefined variables (uninitialized)
            uint64_t absIteration;

            // start!!
            for (absIteration = 0; currDistance2 <= bailout2 && iteration < maxIteration; ++absIteration) {

                assert(refIteration < maxRefIteration);

                const complex<Num> z = perturbationStep(dz, dc0, iteration, refIteration);

                if (tryRebase(z, dz, refIteration, maxRefIteration) &&
                    checkInteriorAndApply(dz, dcMax0, interiorDetectRadius, pdz, pdzIndex)) {
                    return static_cast<double>(isAbs ? absIteration : maxIteration);
                }

                dz = dz.try_normalized_value();

                updateDistance(z, prevDistance2, currDistance2);
                applyMPApproximation(dz, dc0, iteration, refIteration);

                if ((absIteration - 1) % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0 &&
                    state.interruptRequested())
                    return 0.0;
            }

            // refine the result
            if (isAbs)
                return static_cast<double>(absIteration);

            if (iteration >= maxIteration)
                return static_cast<double>(maxIteration);

            const double prevDistance = std::sqrt(prevDistance2);
            const double currDistance = std::sqrt(currDistance2);

            return static_cast<double>(iteration) +
                   FrtDecimalizeIterationMethodUtil::getExteriorDoubleValueIterationRatio(
                           prevDistance, currDistance, ptbSettings.decimalizeIterationMethod, generalSettings.bailout);
        }

        void checkInteriorAndApplyBatch(const SIMDChunk<double> &chunk, std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> &dz,
                                   const float interiorDetectRadius, const bool isAbs, const uint64_t maxIteration,
                                   std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> &finished, const Num &dcMax0,
                                   const std::array<int, RFF_SIMD_CHUNK_SIZE> &absIteration,
                                   std::array<std::array<complex<Num>, PERIODIC_POINT_HISTORY_LENGTH>, RFF_SIMD_CHUNK_SIZE> &pdz,
                                   std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> &pdzIndex, uint32_t i) const noexcept {
            if (finished[i])
                return;

            if (interiorDetectRadius != 0) {
                for (const complex<Num> &pdz0: pdz[i]) {
                    if ((pdz0 - dz[i]).norm_sqr() < dcMax0 * interiorDetectRadius) {
                        *chunk.data[i].value = isAbs ? absIteration[i] : static_cast<double>(maxIteration);
                        finished[i] = true;
                    }
                }

                pdz[i][pdzIndex[i]++] = dz[i];
                if (pdzIndex[i] == pdz[i].size())
                    pdzIndex[i] = 0;
            }
        }

        void iterateBatch(SIMDChunk<double> &chunk, std::array<complex<dex>, RFF_SIMD_CHUNK_SIZE> &dcs) const noexcept override {
            if (state.interruptRequested())
                return;

            std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> dcs0;
            for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                dcs0[i] = static_cast<complex<Num>>(dcs[i] + off);
            }

            std::array<uint64_t, RFF_SIMD_CHUNK_SIZE> iteration;
            std::array<uint64_t, RFF_SIMD_CHUNK_SIZE> refIteration;
            std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> dz;


            if (saSettings.use) {
                std::array<complex<dex>, RFF_SIMD_CHUNK_SIZE> dcSa;
                std::array<complex<dex>, RFF_SIMD_CHUNK_SIZE> dcSaM;
                std::array<complex<dex>, RFF_SIMD_CHUNK_SIZE> dzSa;
                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    dcSa[i] = {.re = dex{dcs0[i].re}, .im = dex{dcs0[i].im}};
                    dcSaM[i] = dcSa[i];
                    dzSa[i] = complex<dex>::ZERO;
                }

                for (const auto term: seriesApproximationData.terms) {
                    for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                        dzSa[i] = (dzSa[i] + term * dcSaM[i]).try_normalized_value();
                        dcSaM[i] = (dcSaM[i] * dcSa[i]).try_normalized_value();
                    }
                }

                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    iteration[i] = seriesApproximationData.skippedIterations;
                    refIteration[i] = seriesApproximationData.skippedIterations;
                    dz[i] = {Num(dzSa[i].re), Num(dzSa[i].im)};
                }
            } else {
                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    iteration[i] = 0;
                    refIteration[i] = 0;
                    dz[i] = complex<Num>::ZERO;
                }
            }
            const uint64_t maxRefIteration = reference.longestPeriod();
            const float interiorDetectRadius =
                    ptbSettings.interiorDetectRadiusPower == 0 ? 0 : pow(10, -ptbSettings.interiorDetectRadiusPower);
            const bool isAbs = ptbSettings.absoluteIterationMode;
            const uint64_t maxIteration = ptbSettings.maxIteration;
            const double bailout2 = generalSettings.bailout * generalSettings.bailout;

            auto dcMax0 = Num(dcMax);

            std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> finished;
            std::array<int, RFF_SIMD_CHUNK_SIZE> absIteration;
            std::array<double, RFF_SIMD_CHUNK_SIZE> currDistance2;
            std::array<double, RFF_SIMD_CHUNK_SIZE> prevDistance2;
            std::array<std::array<complex<Num>, PERIODIC_POINT_HISTORY_LENGTH>, RFF_SIMD_CHUNK_SIZE> pdz;
            std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> pdzIndex;
            std::array<PA<Num>, RFF_SIMD_CHUNK_SIZE> pas;
            std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> oldZ2;
            std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> oldPtbZ;
            std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> z;
            std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> c;
            std::array<double, RFF_SIMD_CHUNK_SIZE> resultCurrDistance2;
            std::array<double, RFF_SIMD_CHUNK_SIZE> resultPrevDistance2;

            for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                finished[i] = 0;
                absIteration[i] = 0;
                currDistance2[i] = 0;
                prevDistance2[i] = 0;
                pdz[i] = {};
                pdzIndex[i] = 0;
                pas[i] = PA<Num>::IDENTITY;
                oldZ2[i] = complex<Num>::ZERO;
                oldPtbZ[i] = complex<Num>::ZERO;
                z[i] = complex<Num>::ZERO;
                c[i] = reference.orbit(1) + dcs0[i];
                resultCurrDistance2[i] = 0;
                resultPrevDistance2[i] = 0;
            }

            for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                if (checkInteriorBasic(c[i])) {
                    *chunk.data[i].value = isAbs ? 1 : static_cast<double>(maxIteration);
                    finished[i] = true;
                }
            }


            const bool noComp = reference.compressor.size() == 0;


            while (std::ranges::any_of(finished, [](const uint8_t a) { return !a; })) {

                table.lookupBatch(pas, refIteration, dz, finished);


                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    const PA<Num> &pa = pas[i];
                    dz[i] = pa.apply(dz[i], dcs0[i]);
                    iteration[i] += pa.skip;
                    refIteration[i] += pa.skip;
                    ++absIteration[i];
                }

                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    if (iteration[i] >= maxIteration) {
                        *chunk.data[i].value = isAbs ? absIteration[i] : maxIteration;
                        finished[i] = true;
                    }
                }


                if (noComp) {
                    for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                        assert(refIteration[i] != maxRefIteration);
                        oldZ2[i] = reference.refOrbit[refIteration[i]] * 2;
                        oldPtbZ[i] = oldZ2[i] + dz[i];
                        dz[i] = (oldPtbZ[i] * dz[i] + dcs0[i]) * (1 - finished[i]);

                        ++refIteration[i];
                        ++iteration[i];
                        ++absIteration[i];

                        z[i] = reference.refOrbit[refIteration[i]] + dz[i];
                        prevDistance2[i] = currDistance2[i];
                        currDistance2[i] = static_cast<complex<double>>(z[i]).norm_sqr();
                    }
                } else {

                    for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                        assert(refIteration[i] != maxRefIteration);
                        oldZ2[i] = reference.orbit(refIteration[i]) * 2;
                        oldPtbZ[i] = oldZ2[i] + dz[i];
                        dz[i] = (oldPtbZ[i] * dz[i] + dcs0[i]) * (1 - finished[i]);

                        ++refIteration[i];
                        ++iteration[i];
                        ++absIteration[i];

                        z[i] = reference.orbit(refIteration[i]) + dz[i];
                        prevDistance2[i] = currDistance2[i];
                        currDistance2[i] = static_cast<complex<double>>(z[i]).norm_sqr();
                    }
                }

                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    if (refIteration[i] == maxRefIteration || z[i].norm_approx() < dz[i].norm_approx()) {
                        refIteration[i] = 0;
                        dz[i] = z[i];
                        checkInteriorAndApplyBatch(chunk, dz, interiorDetectRadius, isAbs, maxIteration, finished, dcMax0,
                                              absIteration, pdz, pdzIndex, i);
                    }
                    dz[i] = dz[i].try_normalized_value();
                }


                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    if (finished[i])
                        continue;
                    if (iteration[i] >= maxIteration) {
                        *chunk.data[i].value = isAbs ? absIteration[i] : static_cast<double>(maxIteration);
                        finished[i] = true;
                    } else if (currDistance2[i] > bailout2) {
                        *chunk.data[i].value = isAbs ? absIteration[i] : static_cast<double>(iteration[i]);
                        resultCurrDistance2[i] = currDistance2[i];
                        resultPrevDistance2[i] = prevDistance2[i];
                        finished[i] = true;
                    }
                }

                for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                    if (absIteration[i] % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0 &&
                        state.interruptRequested())
                        return;
                }
            }

            std::array<double, RFF_SIMD_CHUNK_SIZE> prevDistance;
            std::array<double, RFF_SIMD_CHUNK_SIZE> currDistance;

            for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
                prevDistance[i] = std::sqrt(resultPrevDistance2[i]);
                currDistance[i] = std::sqrt(resultCurrDistance2[i]);


                *chunk.data[i].value += FrtDecimalizeIterationMethodUtil::getExteriorDoubleValueIterationRatio(
                        prevDistance[i], currDistance[i], ptbSettings.decimalizeIterationMethod,
                        generalSettings.bailout);

                finished[i] = true;
            }
        }
    };
} // namespace merutilm::rff2
