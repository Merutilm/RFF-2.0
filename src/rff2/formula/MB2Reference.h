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
        const std::vector<Num> refReal;
        const std::vector<Num> refImag;


        explicit MB2Reference(fixed_point_complex_i1 &&center, std::vector<Num> &&refReal, std::vector<Num> &&refImag,
                              std::vector<ArrayCompressionTool> &&compressor, std::vector<uint64_t> &&period,
                              fixed_point_complex &&fpgReference, fixed_point_complex &&fpgBn);

        static CreationResult generateReference(const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
                                         const FrtReferenceSettings &refSettings, int exp10,
                                         uint64_t refInitialCapacity, uint64_t fixedPeriod, dex dcMax, bool strictFPG,
                                         std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                         std::unique_ptr<MB2Reference> *result);


        [[nodiscard]] Num real(uint64_t refIteration) const;

        [[nodiscard]] Num imag(uint64_t refIteration) const;

        [[nodiscard]] size_t length() const override;
    };


    template<Number Num>
    MB2Reference<Num>::MB2Reference(fixed_point_complex_i1 &&center, std::vector<Num> &&refReal,
                                    std::vector<Num> &&refImag, std::vector<ArrayCompressionTool> &&compressor,
                                    std::vector<uint64_t> &&period, fixed_point_complex &&fpgReference,
                                    fixed_point_complex &&fpgBn) :
        MB2ReferenceBase(std::move(center), std::move(compressor), std::move(period), std::move(fpgReference),
                         std::move(fpgBn)),
        refReal(std::move(refReal)), refImag(std::move(refImag)) {}


    template<Number Num>
    Reference::CreationResult MB2Reference<Num>::generateReference(
            const ParallelRenderState &state, const FrtGeneralSettings &generalSettings,
            const FrtReferenceSettings &refSettings, int exp10, uint64_t refInitialCapacity, uint64_t fixedPeriod,
            dex dcMax, const bool strictFPG, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
            std::unique_ptr<MB2Reference> *result) {
        if (state.interruptRequested()) {
            return CreationResult::TERMINATED;
        }

        auto rr = std::vector<Num>();
        auto ri = std::vector<Num>();

        rr.reserve(refInitialCapacity);
        ri.reserve(refInitialCapacity);

        rr.push_back(Num(0));
        ri.push_back(Num(0));

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
        bool useParallel = refSettings.useParallelRefCalculation;

        Num fpgBnr = Num(1);
        Num fpgBni = Num(0);

        Num zr = Num(0);
        Num zi = Num(0);
        Num cr;
        Num ci;
        cr = calculatable::from_fixed_point_decimal<Num>(c.get_real());
        ci = calculatable::from_fixed_point_decimal<Num>(c.get_imag());

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

        for (period = 0; period == 0 || zr * zr + zi * zi < bailoutSqr; ++period) {
            if (state.interruptRequested()) {
                return CreationResult::TERMINATED;
            }

            // use Fast-Period-Guessing to prepare MPA Table creation
            // fpg
            if (period > 0) {
                Num radius2 = zr * zr + zi * zi;
                Num fpgLimit = radius2 / Num(dcMax);
                Num fpgBnrTemp = calculatable::try_normalized_value(fpgBnr * zr * Num(2) - fpgBni * zi * Num(2) + Num(1));
                Num fpgBniTemp = calculatable::try_normalized_value(fpgBnr * zi * Num(2) + fpgBni * zr * Num(2));


                Num fpgRadius = calculatable::hypot_approx(fpgBnrTemp, fpgBniTemp);
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

                fpgBnr = fpgBnrTemp;
                fpgBni = fpgBniTemp;
            }

            // strict fpg
            if (strictFPG) {
                fixed_point_complex::dbl(temp, z);
                fixed_point_complex::mul(fpgBn, fpgBn, temp, useParallel ? &parallelReferenceThreadPool : nullptr);
                fixed_point_complex::add(fpgBn, fpgBn, one);
            }
            // listener invocation
            func(period);
            fixed_point_complex::sqr(z, z, useParallel ? &parallelReferenceThreadPool : nullptr);
            fixed_point_complex::add(z, z, c);

            // num value
            if (refSyncRadiusPower == 0 || refSyncInterval == 1) {
                zr = calculatable::from_fixed_point_decimal<Num>(z.get_real());
                zi = calculatable::from_fixed_point_decimal<Num>(z.get_imag());
            } else {
                const Num zr2 = (zr + zi) * (zr - zi) + cr;
                const Num zi2 = Num(2) * zr * zi + ci;
                const Num radius2 = zr2 * zr2 + zi2 * zi2;

                if (radius2 < refSyncRadius2 || period % refSyncInterval == 0) {
                    zr = calculatable::from_fixed_point_decimal<Num>(z.get_real());
                    zi = calculatable::from_fixed_point_decimal<Num>(z.get_imag());
                } else {
                    zr = calculatable::try_normalized_value(zr2);
                    zi = calculatable::try_normalized_value(zi2);
                }
            }


            if constexpr(std::is_same_v<dex, Num>) {
                if (calculatable::is_zero(zr)) {
                    zr = dex_exp::exp10(exp10 * Constants::Fractal::INTENTIONAL_ERROR_REFZERO_POWER);
                }
                if (calculatable::is_zero(zi)) {
                    zi = dex_exp::exp10(exp10 * Constants::Fractal::INTENTIONAL_ERROR_REFZERO_POWER);
                }
            }

            uint64_t normalizedPeriodForCompCheck = period;

            if (compressCriteria > 0) {
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
                    const bool sr = calculatable::is_zero(zr) && calculatable::is_zero(rr[refIndex]);
                    const bool si = calculatable::is_zero(zi) == calculatable::is_zero(ri[refIndex]);

                    if ((sr || std::fabs(static_cast<double>(zr / rr[refIndex]) - 1) <= compressionThreshold) &&
                        (si || std::fabs(static_cast<double>(zi / ri[refIndex]) - 1) <= compressionThreshold) &&
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
                if (index == rr.size()) {
                    rr.push_back(zr);
                    ri.push_back(zi);
                } else {
                    rr[index] = zr;
                    ri[index] = zi;
                }
            }
        }


        if (!strictFPG)
            fpgBn = fixed_point_complex(fpgBnr, fpgBni, exp10, strictIntExp10);
        if (fpgReference == nullptr)
            fpgReference = std::make_unique<fixed_point_complex>(z);

        rr.resize(period - compressed + 1);
        ri.resize(period - compressed + 1);
        rr.shrink_to_fit();
        ri.shrink_to_fit();
        periodArray = periodArray.empty() ? std::vector(1, period) : periodArray;

        *result =
                std::make_unique<MB2Reference>(std::move(center), std::move(rr), std::move(ri), std::move(tools),
                                                   std::move(periodArray), std::move(*fpgReference), std::move(fpgBn));

        return CreationResult::SUCCESS;
    }

    template<Number Num>
    Num MB2Reference<Num>::real(const uint64_t refIteration) const {
        return refReal[ArrayCompressor::compress(compressor, refIteration)];
    }
    template<Number Num>
    Num MB2Reference<Num>::imag(const uint64_t refIteration) const {
        return refImag[ArrayCompressor::compress(compressor, refIteration)];
    }

    template<Number Num>
    size_t MB2Reference<Num>::length() const {
        return refReal.size();
    }

    using LightMB2Reference = MB2Reference<double>;
    using DeepMB2Reference = MB2Reference<dex>;

} // namespace merutilm::rff2
