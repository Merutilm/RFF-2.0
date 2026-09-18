//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "../app/FnListeners.hpp"
#include "../calc/fixed_point_complex.hpp"
#include "../mrthy/ArrayCompressionTool.h"
#include "../mrthy/ArrayCompressor.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtReferenceSettings.hpp"
#include "Reference.hpp"
#include "ReferenceCheckpoint.hpp"

namespace merutilm::rff2 {

    struct MB2ReferenceBase {

        const FrtGeneralSettings generalSettings;
        const FrtReferenceSettings refSettings;
        fixed_point_complex center;
        const std::vector<ArrayCompressionTool> compressor;
        const std::vector<uint64_t> period;
        std::vector<ReferenceCheckpoint> checkpoints;
        const complex<dex> fpgBn;
        const float logZoom;
        const dex dcMax;

        MB2ReferenceBase(FrtGeneralSettings generalSettings, FrtReferenceSettings refSettings,
                         fixed_point_complex &&center, std::vector<ArrayCompressionTool> &&compressor,
                         std::vector<uint64_t> &&period, std::vector<ReferenceCheckpoint> &&checkpoints,
                         complex<dex> fpgBn, const float logZoom, const dex dcMax) :
            generalSettings(std::move(generalSettings)), refSettings(std::move(refSettings)), center(std::move(center)),
            compressor(std::move(compressor)), period(std::move(period)), checkpoints(std::move(checkpoints)),
            fpgBn(std::move(fpgBn)), logZoom(logZoom), dcMax(dcMax) {}

        virtual ~MB2ReferenceBase() = default;

        [[nodiscard]] virtual size_t length() const = 0;

        [[nodiscard]] uint64_t longestPeriod() const { return period.back(); }
    };

    template<Number Num>
    struct MB2Reference final : public Reference, public MB2ReferenceBase {
        std::vector<complex<Num>> refOrbit;

        using MB2ReferenceBase::MB2ReferenceBase;

        static void syncReference(fixed_point_complex const &z, uint64_t intervalCounter, uint32_t refSyncInterval,
                                  uint8_t refSyncRadiusPower, Num refSyncRadius2, complex<Num> &z0, complex<Num> &c0);

        static void updatePrecision(int32_t exp10, const fixed_point_complex &cOrig, fixed_point_complex &c,
                                    fixed_point_complex &z, const complex<Num> &fzgAn, int32_t &prevExp2div64);

        static bool fxgABnShouldEscapeWithUpdate(dex dcMax, complex<Num> &fzgAn, complex<Num> &fpgBn, complex<Num> &z0, uint64_t period, Num &radius2);


        static void addCheckpointWithFzgAnStep(std::vector<ReferenceCheckpoint> &checkpoints, fixed_point_complex &z,
                                               complex<Num> &fzgAnPartition, complex<Num> &z0, uint64_t period);

        static void appendOrbit(std::vector<complex<Num>> &ref, const complex<Num> &z0, uint64_t &reuseIndex,
                                 std::vector<ArrayCompressionTool> &tools, uint64_t &compressed,
                                 uint32_t compressCriteria, double compressionThreshold, uint64_t period);

        static void appendCandidatePeriod(std::vector<uint64_t> &periodArray, Num &minZRadius, uint64_t period, Num radius2);

        template<FnListeners::FnRefCalc FnRefCalc>
        static void applyFormula(fixed_point_complex &z, const fixed_point_complex &c, FnRefCalc &&fnRefCalc,
                                 op_thread_pool *sqrTp, uint64_t invoker);

        template<FnListeners::FnRefCalc FnRefCalc>
        static CreationResult
        generateReference(const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                          const FrtReferenceSettings &refSettings, int32_t exp10, uint64_t refInitialCapacity,
                          dex dcMax, FnRefCalc &&fnRefCalc, std::unique_ptr<MB2Reference> *result);


        [[nodiscard]] complex<Num> orbit(uint64_t refIteration) const;

        [[nodiscard]] size_t length() const override;
    };


    template<Number Num>
    void MB2Reference<Num>::syncReference(const fixed_point_complex &z, const uint64_t intervalCounter,
                                          const uint32_t refSyncInterval, const uint8_t refSyncRadiusPower,
                                          const Num refSyncRadius2, complex<Num> &z0, complex<Num> &c0) {

        if (refSyncRadiusPower == 0 || refSyncInterval == 1) {
            z0 = static_cast<complex<Num>>(z);
        } else {
            const complex<Num> next = z0 * z0 + c0;
            const Num radius2 = next.norm_sqr();


            if (radius2 < refSyncRadius2 || intervalCounter % refSyncInterval == 0) {
                z0 = static_cast<complex<Num>>(z);
            } else {
                z0 = next.try_normalized_value();
            }
        }
    }


    template<Number Num>
    void MB2Reference<Num>::updatePrecision(const int32_t exp10, const fixed_point_complex &cOrig, fixed_point_complex &c,
                                            fixed_point_complex &z, const complex<Num> &fzgAn, int32_t &prevExp2div64) {
        const int32_t currentExp10 = std::min(-1, exp10 + static_cast<int>(rff_math::log10Approx(fzgAn.norm_approx())));
        const int32_t exp2div64 = fixed_point_decimal::exp10_to_exp2div64(currentExp10);
        if (prevExp2div64 != exp2div64) {
            z.set_exp10(currentExp10);
            c = cOrig;
            c.set_exp10(currentExp10);
            prevExp2div64 = exp2div64;
        }
    }

    template<Number Num>
    bool MB2Reference<Num>::fxgABnShouldEscapeWithUpdate(const dex dcMax, complex<Num> &fzgAn, complex<Num> &fpgBn, complex<Num> &z0, const uint64_t period, Num &radius2) {
        radius2 = z0.norm_sqr();
        Num fpgLimit = radius2 / Num(dcMax);
        complex<Num> fpgBnTemp = fpgBn * z0 * Num(2) + Num(1);
        Num fpgRadius = fpgBnTemp.norm_approx();

        if (period > 0 && fpgRadius > fpgLimit) {
            return true;
        }
        if (period > 0) {
            fzgAn = (fzgAn * z0 * Num(2)).try_normalized_value();
        }
        fpgBn = fpgBnTemp.try_normalized_value();
        return false;
    }

    template<Number Num>
    void MB2Reference<Num>::addCheckpointWithFzgAnStep(std::vector<ReferenceCheckpoint> &checkpoints,
                                                       fixed_point_complex &z, complex<Num> &fzgAnPartition, complex<Num> &z0,
                                                       const uint64_t period) {
        if (period > 0) {
            fzgAnPartition = (fzgAnPartition * z0 * Num(2)).try_normalized_value();
        }

        if (period % Constants::Fractal::PARTITION_SIZE == 0) {
            checkpoints.emplace_back(z, period, static_cast<complex<dex>>(fzgAnPartition));
            fzgAnPartition = complex<Num>::ONE;
        }
    }

    template<Number Num>
    void MB2Reference<Num>::appendOrbit(std::vector<complex<Num>> &ref, const complex<Num> &z0, uint64_t &reuseIndex,
                                         std::vector<ArrayCompressionTool> &tools, uint64_t &compressed,
                                         const uint32_t compressCriteria, const double compressionThreshold,
                                         const uint64_t period) {
        if (compressCriteria > 0 && period >= 1) {
            const uint64_t refIndex = ArrayCompressor::compress(tools, reuseIndex + 1);
            const bool sr = rff_math::is_zero(z0.re) && rff_math::is_zero(ref[refIndex].re);
            const bool si = rff_math::is_zero(z0.im) && rff_math::is_zero(ref[refIndex].im);

            if ((sr || std::fabs(static_cast<double>(z0.re / ref[refIndex].re) - 1) <= compressionThreshold) &&
                (si || std::fabs(static_cast<double>(z0.im / ref[refIndex].im) - 1) <= compressionThreshold)) {
                ++reuseIndex;
            } else if (reuseIndex != 0) {
                if (reuseIndex > compressCriteria) {
                    // reference compression criteria

                    const auto compressor = ArrayCompressionTool(1, period - reuseIndex + 1, period);
                    compressed += compressor.range(); // get the increment of iteration
                    tools.push_back(compressor);
                }
                // If it is enough to large, set all reference in the range to 0 and save the index

                reuseIndex = 0;
            }
        }


        if (compressCriteria == 0 || reuseIndex <= compressCriteria) {
            const uint64_t index = period - compressed + 1;
            if (index == ref.size()) {
                ref.push_back(z0);
            }
        }
    }

    template<Number Num>
    void MB2Reference<Num>::appendCandidatePeriod(std::vector<uint64_t> &periodArray, Num &minZRadius, const uint64_t period,
                                                  Num radius2) {
        if (period > 0 && minZRadius > radius2) {
            minZRadius = radius2;
            periodArray.push_back(period);
        }
    }
    template<Number Num>
    template<FnListeners::FnRefCalc FnRefCalc>
    void MB2Reference<Num>::applyFormula(fixed_point_complex &z, const fixed_point_complex &c, FnRefCalc &&fnRefCalc,
                                         op_thread_pool *sqrTp, const uint64_t invoker) {
        fnRefCalc(invoker);

        fixed_point_complex::sqr(z, z, sqrTp);
        fixed_point_complex::add(z, z, c);
    }

    template<Number Num>
    template<FnListeners::FnRefCalc FnRefCalc>
    Reference::CreationResult
    MB2Reference<Num>::generateReference(const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                                         const FrtReferenceSettings &refSettings, const int32_t exp10,
                                         uint64_t refInitialCapacity, dex dcMax, FnRefCalc &&fnRefCalc,
                                         std::unique_ptr<MB2Reference> *result) {
        if (state.interruptRequested()) {
            return CreationResult::TERMINATED;
        }

        std::vector<complex<Num>> ref;
        ref.reserve(refInitialCapacity);
        ref.push_back(complex<Num>::ZERO);

        std::vector<ReferenceCheckpoint> checkpoints{};
        checkpoints.reserve(generalSettings.threads + 1);

        fixed_point_complex cOrig = refSettings.center.create_variant(exp10);
        fixed_point_complex c = cOrig;


        auto z = fixed_point_complex(0.0, 0.0, exp10);
        auto bailoutSqr = Num(generalSettings.bailout * generalSettings.bailout);

        op_thread_pool parallelReferenceThreadPoolForRef{};
        op_thread_pool *sqrTp = refSettings.useParallelRefCalculation ? &parallelReferenceThreadPoolForRef : nullptr;

        auto fzgAn = complex<Num>::ONE;
        auto fzgAnPartition = fzgAn;
        auto fpgBn = complex<Num>::ZERO;

        auto z0 = complex<Num>::ZERO;
        auto c0 = static_cast<complex<Num>>(c);

        auto periodArray = std::vector<uint64_t>();

        Num minZRadius = Num(1);

        auto tools = std::vector<ArrayCompressionTool>();

        auto [refSyncInterval, refSyncRadiusPower] = refSettings.sync;
        auto [compressCriteria, compressionThresholdPower] = refSettings.compression;

        const double compressionThreshold = compressionThresholdPower <= 0 ? 0 : pow(10, -compressionThresholdPower);
        Num refSyncRadius2 = Num(pow(10, -refSyncRadiusPower * 2));

        // ReSharper disable once CppTooWideScope
        uint64_t reuseIndex = 0;
        // ReSharper disable once CppTooWideScope
        uint64_t compressed = 0;
        uint64_t period = 0;
        // ReSharper disable once CppTooWideScope
        int32_t prevExp2div64 = 0;

        auto fn = std::forward<FnRefCalc>(fnRefCalc);

        for (period = 0; z0.norm_sqr() < bailoutSqr; ++period) {
            if (state.interruptRequested()) {
                return CreationResult::TERMINATED;
            }

            Num radius2;
            if (fxgABnShouldEscapeWithUpdate(dcMax, fzgAn, fpgBn, z0, period, radius2))
                break;

            addCheckpointWithFzgAnStep(checkpoints, z, fzgAnPartition, z0, period);
            appendCandidatePeriod(periodArray, minZRadius, period, radius2);
            updatePrecision(exp10, cOrig, c, z, fzgAn, prevExp2div64);
            applyFormula(z, c, fn, sqrTp, period);
            syncReference(z, period, refSyncInterval, refSyncRadiusPower, refSyncRadius2, z0, c0);
            appendOrbit(ref, z0, reuseIndex, tools, compressed, compressCriteria, compressionThreshold, period);

        }

        periodArray.push_back(period);
        checkpoints.emplace_back(z, period, static_cast<complex<dex>>(fzgAnPartition));

        *result = std::make_unique<MB2Reference>(generalSettings, refSettings, std::move(cOrig), std::move(tools),
                                                 std::move(periodArray), std::move(checkpoints),
                                                 static_cast<complex<dex>>(fpgBn), generalSettings.logZoom, dcMax);
        (*result)->refOrbit = std::move(ref);
        return CreationResult::SUCCESS;
    }

    template<Number Num>
    complex<Num> MB2Reference<Num>::orbit(const uint64_t refIteration) const {
        return refOrbit[ArrayCompressor::compress(compressor, refIteration)];
    }

    template<Number Num>
    size_t MB2Reference<Num>::length() const {
        return refOrbit.size();
    }
} // namespace merutilm::rff2
