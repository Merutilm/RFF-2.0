//
// Created by Merutilm on 2025-05-18.
//

#pragma once
#include <vector>

#include "../calc/calculatable.hpp"
#include "../calc/fixed_point_complex.hpp"
#include "../mrthy/ArrayCompressionTool.h"
#include "../mrthy/ArrayCompressor.h"
#include "../parallel/ParallelRenderState.h"
#include "../settings/FrtGeneralSettings.hpp"
#include "../settings/FrtReferenceSettings.hpp"
#include "Reference.hpp"

namespace merutilm::rff2 {

    struct MB2ReferenceBase {
        const fixed_point_complex_i1 center;
        const std::vector<ArrayCompressionTool> compressor;
        const std::vector<uint64_t> period;
        const fixed_point_complex fpgReference;
        const fixed_point_complex fpgBn;

        MB2ReferenceBase(fixed_point_complex_i1 &&center, std::vector<ArrayCompressionTool> &&compressor,
                         std::vector<uint64_t> &&period, fixed_point_complex &&fpgReference,
                         fixed_point_complex &&fpgBn) :
            center(std::move(center)), compressor(std::move(compressor)), period(std::move(period)),
            fpgReference(std::move(fpgReference)), fpgBn(std::move(fpgBn)) {}

        virtual ~MB2ReferenceBase() = default;

        [[nodiscard]] virtual size_t length() const = 0;

        [[nodiscard]] uint64_t longestPeriod() const { return period.back(); }
    };

    template<Number Num>
    struct MB2Reference final : public Reference, public MB2ReferenceBase {
        const std::vector<complex<Num>> refOrbit;


        explicit MB2Reference(fixed_point_complex_i1 &&center, std::vector<complex<Num>> &&orbit,
                              std::vector<ArrayCompressionTool> &&compressor, std::vector<uint64_t> &&period,
                              fixed_point_complex &&fpgReference, fixed_point_complex &&fpgBn);

        static void syncReference(fixed_point_complex_i1 &z, uint64_t intervalCounter, uint32_t refSyncInterval,
                                  uint8_t refSyncRadiusPower, Num refSyncRadius2, int exp10, complex<Num> &z0, complex<Num>&c0);

        static void applyFormula(fixed_point_complex_i1 &z, const fixed_point_complex_i1 &c,
                      const std::function<void(uint64_t)> &stepFunc, op_thread_pool *tp, uint64_t invoker);

        static CreationResult generateReference(const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                                         const FrtReferenceSettings &refSettings, int exp10,
                                         uint64_t refInitialCapacity, uint64_t fixedPeriod, dex dcMax, bool strictFPG,
                                         std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                         std::unique_ptr<MB2Reference> *result);


        [[nodiscard]] complex<Num> orbit(uint64_t refIteration) const;

        [[nodiscard]] size_t length() const override;
    };


    template<Number Num>
    MB2Reference<Num>::MB2Reference(fixed_point_complex_i1 &&center, std::vector<complex<Num>> &&orbit, std::vector<ArrayCompressionTool> &&compressor,
                                    std::vector<uint64_t> &&period, fixed_point_complex &&fpgReference,
                                    fixed_point_complex &&fpgBn) :
        MB2ReferenceBase(std::move(center), std::move(compressor), std::move(period), std::move(fpgReference),
                         std::move(fpgBn)),
        refOrbit(std::move(orbit)) {}


    template<Number Num>
    void MB2Reference<Num>::syncReference(fixed_point_complex_i1 &z, const uint64_t intervalCounter, const uint32_t refSyncInterval, const uint8_t refSyncRadiusPower, const Num refSyncRadius2, const int exp10, complex<Num> &z0, complex<Num>&c0) {

        if (refSyncRadiusPower == 0 || refSyncInterval == 1) {
            z0 = {
                calculatable::from_fixed_point_decimal<Num>(z.get_real()),
                calculatable::from_fixed_point_decimal<Num>(z.get_imag())
            };
        } else {
            const complex<Num> next = z0 * z0 + c0;
            const Num radius2 = next.norm_sqr();



            if (radius2 < refSyncRadius2 || intervalCounter % refSyncInterval == 0) {
                z0 = {
                    calculatable::from_fixed_point_decimal<Num>(z.get_real()),
                    calculatable::from_fixed_point_decimal<Num>(z.get_imag())
                };
            } else {

                z0 = next.try_normalized_value();

                // if constexpr(std::is_same_v<Num, double>) {
                //     complex<Num> z2 = {
                //         calculatable::from_fixed_point_decimal<Num>(z.get_real()),
                //         calculatable::from_fixed_point_decimal<Num>(z.get_imag())
                //     };
                //
                //     // if (z0.re/z2.re <0.99 || z0.re/z2.re >1.01 || z0.im/z2.im <0.99 || z0.im/z2.im >1.01) {
                //         std::cout << intervalCounter % refSyncInterval << " | " << z0.re - z2.re  << " " << z0.im - z2.im << "i" << std::endl;
                //     // }
                // }
            }
        }
    }
    template<Number Num>
    void MB2Reference<Num>::applyFormula(fixed_point_complex_i1 &z, const fixed_point_complex_i1 &c, const std::function<void(uint64_t)> &stepFunc, op_thread_pool *tp, const uint64_t invoker) {
        stepFunc(invoker);
        fixed_point_complex::sqr(z, z, tp);
        fixed_point_complex::add(z, z, c);
    }


    template<Number Num>
    Reference::CreationResult MB2Reference<Num>::generateReference(
            const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
            const FrtReferenceSettings &refSettings, int exp10, uint64_t refInitialCapacity, uint64_t fixedPeriod,
            dex dcMax, const bool strictFPG, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
            std::unique_ptr<MB2Reference> *result) {
        if (state.interruptRequested()) {
            return CreationResult::TERMINATED;
        }

        auto ref = std::vector<complex<Num>>();

        ref.reserve(refInitialCapacity);

        ref.push_back(complex<Num>::ZERO);

        int strictIntExp10 = -exp10;
        int fpgIntExp10 = strictFPG ? strictIntExp10 : 1;

        fixed_point_complex_i1 center = refSettings.center;
        fixed_point_complex_i1 c = center.create_variant(exp10);
        auto z = fixed_point_complex_i1(0.0, 0.0, exp10);
        auto temp = z;
        auto fpgBn = fixed_point_complex(0.0, 0.0, exp10, fpgIntExp10);
        auto one = fixed_point_complex_i1(1.0, 0.0, exp10);
        auto bailoutSqr = Num(generalSettings.bailout * generalSettings.bailout);

        op_thread_pool parallelReferenceThreadPool{};
        op_thread_pool *tp = refSettings.useParallelRefCalculation ? &parallelReferenceThreadPool : nullptr;

        complex<Num> fpgBn0 = complex<Num>::ONE;

        complex<Num> z0 = complex<Num>::ZERO;
        complex<Num> c0 = {
            calculatable::from_fixed_point_decimal<Num>(c.get_real()),
            calculatable::from_fixed_point_decimal<Num>(c.get_imag())
        };

        auto periodArray = std::vector<uint64_t>();

        Num minZRadius = Num(1);
        uint64_t reuseIndex = 0;

        auto tools = std::vector<ArrayCompressionTool>();
        uint64_t compressed = 0;

        auto [refSyncInterval, refSyncRadiusPower] = refSettings.sync;
        auto [compressCriteria, compressionThresholdPower, withoutNormalize] = refSettings.compression;

        double compressionThreshold = compressionThresholdPower <= 0 ? 0 : pow(10, -compressionThresholdPower);
        Num refSyncRadius2 = Num(pow(10, -refSyncRadiusPower * 2));
        bool canReuse = withoutNormalize;

        std::unique_ptr<fixed_point_complex> fpgReference = nullptr;
        auto func = std::move(actionPerRefCalcIteration);

        uint64_t period = 0;

        for (period = 0; period == 0 || z0.norm_sqr() < bailoutSqr; ++period) {
            if (state.interruptRequested()) {
                return CreationResult::TERMINATED;
            }

            // use Fast-Period-Guessing to prepare MPA Table creation
            // fpg
            if (period > 0) {
                Num radius2 = z0.norm_sqr();
                Num fpgLimit = radius2 / Num(dcMax);
                complex<Num> fpgBnTemp = fpgBn0 * z0 * Num(2) + Num(1);

                Num fpgRadius = fpgBnTemp.norm_approx();
                bool isRadZero = calculatable::is_zero(radius2);

                if (minZRadius > radius2 && !isRadZero) {
                    minZRadius = radius2;
                    periodArray.push_back(period);
                }

                if ((fpgReference == nullptr && fpgRadius > fpgLimit) || isRadZero ||
                    (fixedPeriod != 0 && fixedPeriod == period)) {
                    periodArray.push_back(period);
                    fpgReference = std::make_unique<fixed_point_complex>(z);
                    break;
                }

                fpgBn0 = fpgBnTemp.try_normalized_value();
            }

            // strict fpg
            if (strictFPG) {
                fixed_point_complex::dbl(temp, z);
                fixed_point_complex::mul(fpgBn, fpgBn, temp, tp);
                fixed_point_complex::add(fpgBn, fpgBn, one);
            }

            applyFormula(z, c, func, tp, period);
            syncReference(z, period, refSyncInterval, refSyncRadiusPower, refSyncRadius2, exp10, z0, c0);


            if (compressCriteria > 0) {
                uint64_t normalizedPeriodForCompCheck = period;

                if (!withoutNormalize) {
                    for (uint64_t i = periodArray.size(); i > 0; --i) {
                        if (compressCriteria >= periodArray[i - 1]) {
                            break;
                        }

                        normalizedPeriodForCompCheck %= periodArray[i - 1];

                        if (normalizedPeriodForCompCheck == 0) {
                            canReuse = true;
                            break;
                        }

                        if (normalizedPeriodForCompCheck == periodArray[i - 1] - 1) {
                            canReuse = false;
                            break;
                        }
                    }
                }

                if (period >= 1) {
                    const uint64_t refIndex = ArrayCompressor::compress(tools, reuseIndex + 1);
                    const bool sr = calculatable::is_zero(z0.re) && calculatable::is_zero(ref[refIndex].re);
                    const bool si = calculatable::is_zero(z0.im) && calculatable::is_zero(ref[refIndex].im);

                    if ((sr || std::fabs(static_cast<double>(z0.re / ref[refIndex].re) - 1) <= compressionThreshold) &&
                        (si || std::fabs(static_cast<double>(z0.im / ref[refIndex].im) - 1) <= compressionThreshold) &&
                        canReuse) {
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
                        canReuse = withoutNormalize;
                    }
                }
            }

            if (compressCriteria == 0 || reuseIndex <= compressCriteria) {
                const uint64_t index = period - compressed + 1;
                if (index == ref.size()) {
                    ref.push_back(z0);
                } else {
                    ref[index] = z0;
                }
            }
        }

        if (!strictFPG)
            fpgBn = fixed_point_complex(fpgBn0.re, fpgBn0.im, exp10, strictIntExp10);
        if (fpgReference == nullptr)
            fpgReference = std::make_unique<fixed_point_complex>(z);

        ref.resize(period - compressed + 1);
        ref.shrink_to_fit();
        periodArray = periodArray.empty() ? std::vector(1, period) : periodArray;

        *result =
                std::make_unique<MB2Reference>(std::move(center), std::move(ref), std::move(tools),
                                                   std::move(periodArray), std::move(*fpgReference), std::move(fpgBn));

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

    using LightMB2Reference = MB2Reference<double>;
    using DeepMB2Reference = MB2Reference<dex>;

} // namespace merutilm::rff2
