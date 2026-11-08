//
// Created by Merutilm on 11/8/26.
//

#pragma once
#include "../app/FnListeners.hpp"
#include "../calc/templates.hpp"
#include "../constants/FractalConstants.hpp"
#include "../mrthy/SRPCApproximator.hpp"
#include "../mrthy/SRPCProcessor.hpp"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtReferenceSettings.hpp"
#include "MB2ReferenceGenerationResult.hpp"
#include "RFFResult.hpp"
#include "ReferenceCheckpoint.hpp"

namespace merutilm::rff2 {


    enum class SpecialCenterType {
        NONE,
        /**
         * -2 + 0i
         */
        M2,
        /**
         * 0 +- 1i
         */
        PM1I
    };

    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    class MB2ReferenceGenerator {

        // input value
        const ParallelRenderState &state;
        const FrtGeneralSettings &generalSettings;
        const FrtReferenceSettings &refSettings;
        const fixed_point_complex cOrig;
        const int32_t exp10;
        const size_t refInitialCapacity;
        const dex dcMax;
        FnRefCalc fnRefCalc;
        Num refSyncRadius2 = Num(pow(10.0, -refSettings.sync.referenceSynchronizationRadiusPower * 2));


        // processing value
        std::unique_ptr<op_thread_pool> sqrTp =
                refSettings.useParallelRefCalculation ? std::make_unique<op_thread_pool>() : nullptr;
        itt::ref refIteration = 0;
        std::array<fixed_point_decimal, fixed_point_complex::TEMPS_COUNT> temps{};
        std::vector<ReferenceCheckpoint> checkpoints{};
        fixed_point_complex z = fixed_point_complex(0, 0, exp10);
        fixed_point_complex c = cOrig;
        complex<Num> z0 = complex<Num>::ZERO;
        complex<Num> c0 = static_cast<complex<Num>>(c);
        complex<Num> fzgAn = complex<Num>::ONE;
        complex<Num> fzgAnPartition = fzgAn;
        complex<Num> fpgBn = complex<Num>::ZERO;
        std::vector<itt::ref> periodArray = std::vector<itt::ref>();

    public:
        explicit MB2ReferenceGenerator(const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                                       const FrtReferenceSettings &refSettings, int32_t exp10,
                                       size_t refInitialCapacity, dex dcMax, FnRefCalc &&fnRefCalc);


        RFFResult generate(MB2ReferenceGenerationResult<Num> &result);

    private:
        void syncReference();

        void updatePrecision(int32_t &prevExp2div64);

        bool tryUpdateAnBn(Num &radius2);

        void placeCheckpoint();

        void tryAppendPeriodCandidate(Num &minZRadius, Num radius2);
        SpecialCenterType resolveSpecialCenterType() const;

        void applyFormula();

        void stepOnceOptimal(int32_t &prevExp2div64, SpecialCenterType specialCenterType);
    };
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    MB2ReferenceGenerator<Num, FnRefCalc>::MB2ReferenceGenerator(const ParallelRenderState &state,
                                                                 const FrtGeneralSettings &generalSettings,
                                                                 const FrtReferenceSettings &refSettings,
                                                                 const int32_t exp10, const size_t refInitialCapacity,
                                                                 dex dcMax, FnRefCalc &&fnRefCalc) :
        state(state), generalSettings(generalSettings), refSettings(refSettings),
        cOrig(refSettings.center.create_variant(exp10)), exp10(exp10), refInitialCapacity(refInitialCapacity),
        dcMax(std::move(dcMax)), fnRefCalc(std::move(fnRefCalc)) {}

    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    void MB2ReferenceGenerator<Num, FnRefCalc>::syncReference() {

        const auto &[refSyncInterval, refSyncRadiusPower] = refSettings.sync;
        if (refSyncRadiusPower == 0 || refSyncInterval == 1) {
            z0 = static_cast<complex<Num>>(z);
        } else {
            const complex<Num> next = z0 * z0 + c0;
            const Num radius2 = next.norm_sqr();


            if (radius2 < refSyncRadius2 || refIteration % refSyncInterval == 0) {
                z0 = static_cast<complex<Num>>(z);
            } else {
                z0 = next.try_normalized_value();
            }
        }
    }
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    void MB2ReferenceGenerator<Num, FnRefCalc>::updatePrecision(int32_t &prevExp2div64) {
        const int32_t currentExp10 = std::min(-1, exp10 + static_cast<int>(rff_math::log10Approx(fzgAn.max())));
        const int32_t exp2div64 = fixed_point_decimal::exp10_to_exp2div64(currentExp10);
        if (prevExp2div64 != exp2div64) {
            z.set_exp10(currentExp10);
            c = cOrig;
            c.set_exp10(currentExp10);
            for (auto &temp: temps) {
                temp.set_exp10(currentExp10, false);
            }
            prevExp2div64 = exp2div64;
        }
    }
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    bool MB2ReferenceGenerator<Num, FnRefCalc>::tryUpdateAnBn(Num &radius2) {
        radius2 = z0.norm_sqr();
        Num fpgLimit = radius2 / Num(dcMax);
        complex<Num> fpgBnTemp = fpgBn * z0 * Num(2) + Num(1);
        Num fpgRadius = fpgBnTemp.max();

        if (refIteration > 0 && fpgRadius > fpgLimit) {
            return false;
        }
        if (refIteration > 0) {
            fzgAn = (fzgAn * z0 * Num(2)).try_normalized_value();
            fzgAnPartition = (fzgAnPartition * z0 * Num(2)).try_normalized_value();
        }
        fpgBn = fpgBnTemp.try_normalized_value();
        return true;
    }
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    void MB2ReferenceGenerator<Num, FnRefCalc>::placeCheckpoint() {
        checkpoints.emplace_back(z, refIteration, static_cast<complex<dex>>(fzgAnPartition));
        fzgAnPartition = complex<Num>::ONE;
    }
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    void MB2ReferenceGenerator<Num, FnRefCalc>::tryAppendPeriodCandidate(Num &minZRadius, Num radius2) {
        if (refIteration > 0 && minZRadius > radius2) {
            minZRadius = radius2;
            periodArray.push_back(refIteration);
        }
    }
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    SpecialCenterType MB2ReferenceGenerator<Num, FnRefCalc>::resolveSpecialCenterType() const {
        if (c.imag.size == 0 && c.real.compare(-2) == 0) {
            return SpecialCenterType::M2;
        }

        if (c.real.size == 0 && c.imag.compare_abs(1) == 0) {
            return SpecialCenterType::PM1I;
        }
        return SpecialCenterType::NONE;
    }
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    void MB2ReferenceGenerator<Num, FnRefCalc>::applyFormula() {

        fixed_point_complex::sqr(z, z, temps, sqrTp.get());
        fixed_point_complex::add(z, z, c);
    }
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    void MB2ReferenceGenerator<Num, FnRefCalc>::stepOnceOptimal(int32_t &prevExp2div64,
                                                                const SpecialCenterType specialCenterType) {

        switch (specialCenterType) {
            using enum SpecialCenterType;
            case NONE:
                // single-threaded for this method
                updatePrecision(prevExp2div64);
                applyFormula();
                syncReference();
                break;
            case M2: {

                const bool shouldApplyZForCheckpoint =
                        refIteration % refSettings.partitionSize == refSettings.partitionSize - 1;

                z.set_exp10(-1);
                if (refIteration == 0) {
                    if (shouldApplyZForCheckpoint)
                        z.real.set(2, true);
                    z0 = complex<Num>(Num(-2), Num(0));
                } else if (refIteration == 1) {
                    if (shouldApplyZForCheckpoint)
                        z.real.neg();
                    z0 = complex<Num>(Num(2), Num(0));
                }
                break;
            }
            case PM1I: {

                const bool shouldApplyZForCheckpoint =
                        refIteration % refSettings.partitionSize == refSettings.partitionSize - 1;
                z.set_exp10(-1);
                int32_t sgn = c.imag.sgn();
                if (refIteration == 0) {
                    if (shouldApplyZForCheckpoint) {
                        z.real.set(0, true);
                        z.imag.set(sgn, true);
                    }
                    z0 = complex<Num>(Num(0), Num(sgn));
                } else if (refIteration & 1u) {
                    if (shouldApplyZForCheckpoint) {
                        z.real.set(-1, false);
                        z.imag.set(sgn, false);
                    }
                    z0 = complex<Num>(Num(-1), Num(sgn));
                } else {
                    if (shouldApplyZForCheckpoint) {
                        z.real.set(0, false);
                        z.imag.set(-sgn, false);
                    }
                    z0 = complex<Num>(Num(0), Num(-sgn));
                }
                break;
            }
        }
    }
    template<Number Num, FnListeners::FnRefCalc FnRefCalc>
    RFFResult MB2ReferenceGenerator<Num, FnRefCalc>::generate(MB2ReferenceGenerationResult<Num> &result) {
        if (state.interruptRequested()) {
            return RFFResult::TERMINATED;
        }

        checkpoints.reserve(generalSettings.threads + 1);
        z.try_realloc_inc(-z.real.exp2div64 * 2 + 2);

        auto bailoutSqr = Num(generalSettings.bailout * generalSettings.bailout);


        Num minZRadius = Num(1);
        auto [minCompressSize, compressionThresholdPower] = refSettings.compression;

        const double compressionThreshold = compressionThresholdPower <= 0 ? 0 : pow(10, -compressionThresholdPower);

        SRPCProcessor<itt::ref, complex<Num>, SRPCApproximator<Num>> processor(minCompressSize, refInitialCapacity,
                                                                     SRPCApproximator<Num>(compressionThreshold));
        processor.put(complex<Num>::ZERO);


        int32_t prevExp2div64 = 0;

        auto fn = std::forward<FnRefCalc>(fnRefCalc);
        const SpecialCenterType specialCenterType = resolveSpecialCenterType();
        Num radius2;


        for (refIteration = 0; tryUpdateAnBn(radius2) && z0.norm_sqr() < bailoutSqr; ++refIteration) {
            if (refIteration % Constants::Fractal::HOTPATH_INTERRUPT_CHECK_INTERVAL == 0 &&
                state.interruptRequested()) {
                return RFFResult::TERMINATED;
            }

            tryAppendPeriodCandidate(minZRadius, radius2);

            if (refIteration % refSettings.partitionSize == 0)
                placeCheckpoint();

            fn(refIteration);
            stepOnceOptimal(prevExp2div64, specialCenterType);
            processor.put(z0);
        }

        periodArray.push_back(refIteration);
        checkpoints.emplace_back(z, refIteration, static_cast<complex<dex>>(fzgAnPartition));

        result = MB2ReferenceGenerationResult<Num>{.center = std::move(cOrig),
                                                 .generatedPeriods = std::move(periodArray),
                                                 .fpgBn = std::move(fpgBn),
                                                 .checkpoints = std::move(checkpoints),
                                                 .srpcData = std::move(processor).finish()};
        return RFFResult::SUCCESS;
    }
} // namespace merutilm::rff2
