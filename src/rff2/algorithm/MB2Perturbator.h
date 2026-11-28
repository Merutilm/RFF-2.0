//
// Created by Merutilm on 2025-05-18.
//

#pragma once

#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtPerturbSettings.hpp"
#include "MB2MPAGenerator.hpp"
#include "Perturbator.h"

#include "../parallel/SIMDChunk.hpp"
#include "../parallel/SIMDChunkData.hpp"

namespace merutilm::rff2 {


    struct MB2PerturbatorBase : Perturbator {

        ParallelRenderState &state;
        complex<dex> off = complex<dex>::ZERO;

    protected:
        const FrtGeneralSettings &generalSettings;
        const FrtPerturbSettings &ptbSettings;

    public:
        MB2PerturbatorBase(ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                           const FrtPerturbSettings &ptbSettings) :
            state(state), generalSettings(generalSettings), ptbSettings(ptbSettings) {}

        ~MB2PerturbatorBase() override = default;

        [[nodiscard]] virtual double iterate(complex<dex> dc) const noexcept = 0;

        virtual void iterateBatch(SIMDChunk<double> &chunk,
                                  std::array<complex<dex>, RFF_SIMD_CHUNK_SIZE> &dcs) const noexcept = 0;
    };

    template<Number Num>
    struct MB2Perturbator final : MB2PerturbatorBase {

        static constexpr uint32_t PERIODIC_POINT_HISTORY_LENGTH = 8;
        const ReferenceData<Num> &reference;
        const MPAData<Num> &table;

        explicit MB2Perturbator(ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                                const FrtPerturbSettings &ptbSettings, const ReferenceData<Num> &reference,
                                const MPAData<Num> &table) :
            MB2PerturbatorBase(state, generalSettings, ptbSettings), reference(reference), table(table) {}

        ~MB2Perturbator() override = default;

        bool innerCardioidTest(const complex<Num> &dc) const {
            // fast calculation of period-n bulb
            const auto cOff =
                    static_cast<complex<dex>>(dc) - static_cast<complex<dex>>(reference.hnreList.mbNucleusOff);
            const auto cOffSqr = cOff.norm_sqr();
            const auto s = reference.hnreList.scale * 4;
            const auto rot = reference.hnreList.mbRotation;
            const auto cos = rff_math::cos(rot);
            const auto sin = rff_math::sin(rot);

            const auto rad2 = s * s * cOffSqr;
            const auto t2 = s * (cOff.re * cos - cOff.im * sin);
            return rad2 * rad2 < 6 * rad2 + 8 * t2 + 3;
        }

        [[nodiscard]] bool checkInteriorBasic(const complex<Num> &c, const complex<Num> &dc) const {
            if (rff_math::is_zero(c.im) && c.re < Num(0.25) && c.re >= Num(-2))
                return true;

            const auto cr = static_cast<double>(c.re);
            const auto ci = static_cast<double>(c.im);

            // fast calculation of main bulb
            const auto crm025 = cr - 0.25;
            const auto crm025rs = crm025 * crm025 + ci * ci;
            if (crm025rs < 0.5 * (std::sqrt(crm025rs) - crm025))
                return true;

            if (const auto crp100 = cr + 1; crp100 * crp100 + ci * ci < 0.0625)
                return true;


            return false;
        }

        bool checkInteriorAndApply(const complex<Num> &dz, const Num scale, const float interiorDetectRadius,
                                   std::array<complex<Num>, PERIODIC_POINT_HISTORY_LENGTH> &pdz, int &pdzIndex) const {
            if (interiorDetectRadius == 0)
                return false;

            for (const complex<Num> &pdz0: pdz) {
                Num rad = (pdz0 - dz).max();
                if (rad * rad < scale * interiorDetectRadius) {
                    return true;
                }
            }

            pdz[pdzIndex++] = dz;
            if (pdzIndex == pdz.size())
                pdzIndex = 0;

            return false;
        }
        void applyMPApproximation(HNREReferenceView<Num> &referenceView, HNREMPAView<Num> &mpaView, complex<Num> &dz,
                                  const complex<Num> &dc, itt::ptb &iteration, itt::ref &refIteration) const {
            if (const PA<Num> *paPtr = mpaView.lookup(referenceView, refIteration, dz); paPtr != nullptr) {
                const PA<Num> &pa = *paPtr;

                dz = pa.apply(dz, dc);
                iteration += pa.skip;
                refIteration += pa.skip;
            }
        }


        static void updateDistance(const complex<Num> &z, double &prevDistance2, double &currDistance2) {
            prevDistance2 = currDistance2;
            currDistance2 = static_cast<complex<double>>(z).norm_sqr();
        }

        static bool tryRebase(HNREReferenceView<Num> &view, const complex<Num> &z, complex<Num> &dz,
                              itt::ref &refIteration, const itt::ref maxRefIteration) {
            if (refIteration == maxRefIteration || z.max() < dz.max()) {
                view.reset();
                refIteration = 0;
                dz = z;
                return true;
            }
            return false;
        }

        static void perturbationStep(HNREReferenceView<Num> &view, complex<Num> &dz, const complex<Num> &dc,
                                     itt::ptb &iteration, itt::ref &refIteration) {
            dz = (view.get() * 2 + dz) * dz + dc;
            view.next();

            ++iteration;
            ++refIteration;
        }

        [[nodiscard]] double iterate(complex<dex> dc) const noexcept override {
            double iteration = 0;
            const bool isAbs = ptbSettings.absoluteIterationMode;
            for (size_t i = 0; i < table.hnreList.size() - 1; ++i) {
                dc *= table.hnreList[i].hnreTarget->dcTransform;
            }
            complex<Num> dz = complex<Num>::ZERO;
            for (size_t i = table.hnreList.size(); i > 0; --i) {
                const HNREMPAData<Num> &data = table.hnreList[i - 1];

                if (!isAbs)
                    iteration *= data.hnreTarget->longestPeriod();
                if (iteration >= ptbSettings.maxIteration)
                    return isAbs ? iteration : ptbSettings.maxIteration;

                itt::ptb maxIteration = i == 1 ? ptbSettings.maxIteration
                                               : data.hnreTarget->longestPeriod() * ptbSettings.autoIterationMultiplier;
                iteration += hnreIterate(dz, data, dc, maxIteration, i > 1);

                if (i > 1) {
                    dz /= static_cast<complex<Num>>(table.hnreList[i - 2].hnreTarget->dzTransform);
                    dc /= table.hnreList[i - 2].hnreTarget->dcTransform;
                }
            }
            return iteration;
        }

        [[nodiscard]] double hnreIterate(complex<Num> &dz, const HNREMPAData<Num> &data, const complex<dex> &dc,
                                         const itt::ptb maxIteration, const bool smallDzRequired) const noexcept {
            if (state.interruptRequested())
                return 0.0;

            // field reference
            complex<Num> smallZStore = dz;
            itt::ptb smallDzIterationStore = 0;

            const auto dc0 = static_cast<complex<Num>>(dc + off);
            const Num scale = data.hnreTarget->scale;
            const bool isAbs = ptbSettings.absoluteIterationMode;
            const itt::ref maxRefIteration = data.hnreTarget->longestPeriod();
            const float interiorDetectRadius =
                    ptbSettings.interiorDetectRadiusPower == 0 ? 0 : pow(10, -ptbSettings.interiorDetectRadiusPower);
            const double bailout2 = generalSettings.bailout * generalSettings.bailout;

            // basic interior detection
            HNREReferenceView<Num> referenceView = data.hnreTarget->makeReferenceView();
            std::optional<HNREMPAView<Num>> mpaView = std::nullopt;
            if (data.buffer)
                mpaView.emplace(data.makeMPAView());
            referenceView.next();
            if (checkInteriorBasic(referenceView.get() + dc0, dc0)) {
                return isAbs ? 1 : static_cast<double>(maxIteration);
            }
            referenceView.reset();

            // generate initial iteration and dz
            itt::ptb iteration = 0;
            itt::ref refIteration = 0;

            // init local variables
            std::array<complex<Num>, PERIODIC_POINT_HISTORY_LENGTH> pdz = {};
            int pdzIndex = 0;
            double currDistance2 = 0;
            double prevDistance2 = 0;

            // predefined variables (uninitialized)
            itt::ptb absIteration;
            // start!!
            for (absIteration = 0; currDistance2 <= bailout2 && iteration < maxIteration; ++absIteration) {

                assert(refIteration < maxRefIteration);
                perturbationStep(referenceView, dz, dc0, iteration, refIteration);

                const complex<Num> z = referenceView.get() + dz;
                if (tryRebase(referenceView, z, dz, refIteration, maxRefIteration) &&
                    checkInteriorAndApply(dz, scale, interiorDetectRadius, pdz, pdzIndex)) {
                    return static_cast<double>(isAbs ? absIteration : maxIteration);
                }

                dz = dz.try_normalized_value();

                updateDistance(z, prevDistance2, currDistance2);
                if (currDistance2 < Constants::Fractal::HNRE_Z_RADIUS) {
                    smallZStore = z;
                    smallDzIterationStore = iteration;
                }

                if (mpaView)
                    applyMPApproximation(referenceView, *mpaView, dz, dc0, iteration, refIteration);

                if ((absIteration - 1) % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0 &&
                    state.interruptRequested())
                    return 0.0;
            }

            // refine the result
            if (isAbs)
                return static_cast<double>(absIteration);

            if (iteration >= maxIteration)
                return static_cast<double>(maxIteration);

            if (smallDzRequired) {
                dz = smallZStore;
                return smallDzIterationStore;
            }else {
                const double prevDistance = std::sqrt(prevDistance2);
                const double currDistance = std::sqrt(currDistance2);
                const double inc = ptbSettings.smoothIteration
                                           ? FrtDecimalizeIterationMethodUtil::getExteriorDoubleValueIterationRatio(
                                                     prevDistance, currDistance, generalSettings.bailout)
                                           : 0;
                return static_cast<double>(iteration) + inc;
            }

        }

        // void checkInteriorAndApplyBatch(
        //         const SIMDChunk<double> &chunk, std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> &dz,
        //         const float interiorDetectRadius, const bool isAbs, const itt::ptb maxIteration,
        //         std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> &finished, const Num &scale0,
        //         const std::array<itt::ptb, RFF_SIMD_CHUNK_SIZE> &absIteration,
        //         std::array<std::array<complex<Num>, PERIODIC_POINT_HISTORY_LENGTH>, RFF_SIMD_CHUNK_SIZE> &pdz,
        //         std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> &pdzIndex, uint32_t i) const noexcept {
        //     if (finished[i])
        //         return;
        //
        //     if (interiorDetectRadius != 0) {
        //         for (const complex<Num> &pdz0: pdz[i]) {
        //             if ((pdz0 - dz[i]).norm_sqr() < scale0 * interiorDetectRadius) {
        //                 *chunk.data[i].value = isAbs ? absIteration[i] : static_cast<double>(maxIteration);
        //                 finished[i] = true;
        //             }
        //         }
        //
        //         pdz[i][pdzIndex[i]++] = dz[i];
        //         if (pdzIndex[i] == pdz[i].size())
        //             pdzIndex[i] = 0;
        //     }
        // }

        void iterateBatch(SIMDChunk<double> &chunk,
                          std::array<complex<dex>, RFF_SIMD_CHUNK_SIZE> &dcs) const noexcept override {
            if (state.interruptRequested())
                return;
            // TODO Change to AVX512 instructions
            // std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> dcs0;
            // for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //     dcs0[i] = static_cast<complex<Num>>(dcs[i] + off);
            // }
            //
            // std::array<itt::ptb, RFF_SIMD_CHUNK_SIZE> iteration;
            // std::array<itt::ref, RFF_SIMD_CHUNK_SIZE> refIteration;
            // std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> dz;
            // SRPCCursor<itt::ref, complex<Num>> cursor = reference.makeReferenceView();
            //
            // for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //     iteration[i] = 0;
            //     refIteration[i] = 0;
            //     dz[i] = complex<Num>::ZERO;
            // }
            //
            // const itt::ref maxRefIteration = reference.longestPeriod();
            // const float interiorDetectRadius =
            //         ptbSettings.interiorDetectRadiusPower == 0 ? 0 : pow(10, -ptbSettings.interiorDetectRadiusPower);
            // const bool isAbs = ptbSettings.absoluteIterationMode;
            // const itt::ptb maxIteration = ptbSettings.maxIteration;
            // const double bailout2 = generalSettings.bailout * generalSettings.bailout;
            //
            // auto scale0 = Num(scale);
            //
            // std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> finished;
            // std::array<itt::ptb, RFF_SIMD_CHUNK_SIZE> absIteration;
            // std::array<double, RFF_SIMD_CHUNK_SIZE> currDistance2;
            // std::array<double, RFF_SIMD_CHUNK_SIZE> prevDistance2;
            // std::array<std::array<complex<Num>, PERIODIC_POINT_HISTORY_LENGTH>, RFF_SIMD_CHUNK_SIZE> pdz;
            // std::array<uint8_t, RFF_SIMD_CHUNK_SIZE> pdzIndex;
            // std::array<PA<Num>, RFF_SIMD_CHUNK_SIZE> pas;
            // std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> oldZ2;
            // std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> oldPtbZ;
            // std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> z;
            // std::array<complex<Num>, RFF_SIMD_CHUNK_SIZE> c;
            // std::array<double, RFF_SIMD_CHUNK_SIZE> resultCurrDistance2;
            // std::array<double, RFF_SIMD_CHUNK_SIZE> resultPrevDistance2;
            //
            // for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //     finished[i] = 0;
            //     absIteration[i] = 0;
            //     currDistance2[i] = 0;
            //     prevDistance2[i] = 0;
            //     pdz[i] = {};
            //     pdzIndex[i] = 0;
            //     pas[i] = PA<Num>::IDENTITY;
            //     oldZ2[i] = complex<Num>::ZERO;
            //     oldPtbZ[i] = complex<Num>::ZERO;
            //     z[i] = complex<Num>::ZERO;
            //     c[i] = cursor.get(1) + dcs0[i];
            //     resultCurrDistance2[i] = 0;
            //     resultPrevDistance2[i] = 0;
            // }
            //
            // for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //     if (checkInteriorBasic(c[i])) {
            //         *chunk.data[i].value = isAbs ? 1 : static_cast<double>(maxIteration);
            //         finished[i] = true;
            //     }
            // }
            //
            //
            // while (std::ranges::any_of(finished, [](const uint8_t a) { return !a; })) {
            //
            //     table.lookupBatch(pas, refIteration, dz, finished);
            //
            //
            //     for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //         const PA<Num> &pa = pas[i];
            //         dz[i] = pa.apply(dz[i], dcs0[i]);
            //         iteration[i] += pa.skip;
            //         refIteration[i] += pa.skip;
            //         ++absIteration[i];
            //     }
            //
            //     for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //         if (iteration[i] >= maxIteration) {
            //             *chunk.data[i].value = isAbs ? absIteration[i] : maxIteration;
            //             finished[i] = true;
            //         }
            //     }
            //
            //
            //     for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //         assert(refIteration[i] != maxRefIteration);
            //         oldZ2[i] = cursor.get(refIteration[i]) * 2;
            //         oldPtbZ[i] = oldZ2[i] + dz[i];
            //         dz[i] = (oldPtbZ[i] * dz[i] + dcs0[i]) * (1 - finished[i]);
            //
            //         ++refIteration[i];
            //         ++iteration[i];
            //         ++absIteration[i];
            //
            //         z[i] = cursor.get(refIteration[i]) + dz[i];
            //         prevDistance2[i] = currDistance2[i];
            //         currDistance2[i] = static_cast<complex<double>>(z[i]).norm_sqr();
            //     }
            //
            //
            //     for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //         if (refIteration[i] == maxRefIteration || z[i].max() < dz[i].max()) {
            //             refIteration[i] = 0;
            //             dz[i] = z[i];
            //             checkInteriorAndApplyBatch(chunk, dz, interiorDetectRadius, isAbs, maxIteration, finished,
            //                                        dcMax0, absIteration, pdz, pdzIndex, i);
            //         }
            //         dz[i] = dz[i].try_normalized_value();
            //     }
            //
            //
            //     for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //         if (finished[i])
            //             continue;
            //         if (iteration[i] >= maxIteration) {
            //             *chunk.data[i].value = isAbs ? absIteration[i] : static_cast<double>(maxIteration);
            //             finished[i] = true;
            //         } else if (currDistance2[i] > bailout2) {
            //             *chunk.data[i].value = isAbs ? absIteration[i] : static_cast<double>(iteration[i]);
            //             resultCurrDistance2[i] = currDistance2[i];
            //             resultPrevDistance2[i] = prevDistance2[i];
            //             finished[i] = true;
            //         }
            //     }
            //
            //     for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //         if (absIteration[i] % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0 &&
            //             state.interruptRequested())
            //             return;
            //     }
            // }
            //
            // std::array<double, RFF_SIMD_CHUNK_SIZE> prevDistance;
            // std::array<double, RFF_SIMD_CHUNK_SIZE> currDistance;
            //
            // for (uint32_t i = 0; i < RFF_SIMD_CHUNK_SIZE; ++i) {
            //     prevDistance[i] = std::sqrt(resultPrevDistance2[i]);
            //     currDistance[i] = std::sqrt(resultCurrDistance2[i]);
            //
            //
            //     *chunk.data[i].value += FrtDecimalizeIterationMethodUtil::getExteriorDoubleValueIterationRatio(
            //             prevDistance[i], currDistance[i], ptbSettings.decimalizeIterationCurvature,
            //             generalSettings.bailout);
            //
            //     finished[i] = true;
            // }
        }
    };
} // namespace merutilm::rff2
