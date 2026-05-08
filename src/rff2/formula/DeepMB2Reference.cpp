//
// Created by Merutilm on 2025-05-18.
//

#include "DeepMB2Reference.h"

#include "../calc/double_exp_math.h"
#include "../calc/rff_math.h"
#include "../constants/Constants.hpp"
#include "../mrthy/ArrayCompressor.h"


namespace merutilm::rff2 {
    DeepMB2Reference::DeepMB2Reference(fixed_point_complex_i1 &&center, std::vector<dex> &&refReal,
                                       std::vector<dex> &&refImag, std::vector<ArrayCompressionTool> &&compressor,
                                       std::vector<uint64_t> &&period, fixed_point_complex &&fpgReference,
                                       fixed_point_complex &&fpgBn) :
        MB2Reference(std::move(center), std::move(compressor), std::move(period), std::move(fpgReference),
                     std::move(fpgBn)),
        refReal(std::move(refReal)), refImag(std::move(refImag)) {}


    Reference::CreationResult
    DeepMB2Reference::createReference(const ParallelRenderState &state, const FractalSettings &calc, int exp10,
                                      uint64_t refInitialCapacity, uint64_t fixedPeriod, dex dcMax,
                                      const bool strictFPG, std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                      std::unique_ptr<DeepMB2Reference> *result) {
        if (state.interruptRequested()) {
            return CreationResult::TERMINATED;
        }

        auto rr = std::vector<dex>();
        auto ri = std::vector<dex>();

        rr.reserve(refInitialCapacity);
        ri.reserve(refInitialCapacity);

        rr.push_back(dex::ZERO);
        ri.push_back(dex::ZERO);

        int strictIntExp10 = -exp10;
        int fpgIntExp10 = strictFPG ? strictIntExp10 : 1;

        fixed_point_complex_i1 center = calc.center;
        fixed_point_complex_i1 c = center.create_variant(exp10);
        auto z = fixed_point_complex_i1(0.0, 0.0, exp10);
        auto temp = z;
        auto fpgBn = fixed_point_complex(0.0, 0.0, exp10, fpgIntExp10);
        auto one = fixed_point_complex_i1(1.0, 0.0, exp10);
        auto bailoutSqr = dex(calc.bailout * calc.bailout);

        op_thread_pool parallelReferenceThreadPool{};
        bool useParallel = calc.useParallelRefCalculation;

        dex fpgBnr = dex::ONE;
        dex fpgBni = dex::ZERO;

        dex zr = dex::ZERO;
        dex zi = dex::ZERO;
        dex cr;
        dex ci;
        auto two = dex(2);
        cr = c.get_real().dex_value();
        ci = c.get_imag().dex_value();

        auto periodArray = std::vector<uint64_t>();

        dex minZRadius = dex::ONE;
        uint64_t reuseIndex = 0;

        auto tools = std::vector<ArrayCompressionTool>();
        uint64_t compressed = 0;

        auto [refSyncInterval, refSyncRadiusPower] = calc.referenceSyncSettings;
        auto [compressCriteria, compressionThresholdPower, withoutNormalize] = calc.referenceCompSettings;

        double compressionThreshold = compressionThresholdPower <= 0 ? 0 : pow(10, -compressionThresholdPower);
        dex refSyncRadius2 = dex_exp::exp10(-refSyncRadiusPower * 2);
        bool canReuse = withoutNormalize;

        std::unique_ptr<fixed_point_complex> fpgReference = nullptr;
        auto temps = std::array<dex, 8>();
        auto func = std::move(actionPerRefCalcIteration);

        uint64_t period = 0;

        for (period = 0; period == 0 || zr * zr + zi * zi < bailoutSqr; ++period) {
            if (state.interruptRequested()) {
                return CreationResult::TERMINATED;
            }

            // use Fast-Period-Guessing to prepare MPA Table creation
            // fpg
            if (period > 0) {
                dex radius2 = zr * zr + zi * zi;
                dex fpgLimit = radius2 / dcMax;
                dex fpgBnrTemp = fpgBnr * zr * two - fpgBni * zi * two + dex::ONE;
                dex fpgBniTemp = fpgBnr * zi * two + fpgBni * zr * two;

                fpgBnrTemp.try_normalize();
                fpgBniTemp.try_normalize();

                dex fpgRadius = dex_trig::hypot_approx(fpgBnrTemp, fpgBniTemp);
                bool isRadZero = radius2.is_zero();

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

            // dex value
            if (refSyncRadiusPower == 0 || refSyncInterval == 1) {
                zr = z.get_real().dex_value();
                zi = z.get_imag().dex_value();
            } else {
                const dex zr2 = (zr + zi) * (zr - zi) + cr;
                const dex zi2 = two * zr * zi + ci;
                const dex radius2 = zr2 * zr2 + zi2 * zi2;

                if (radius2 < refSyncRadius2 || period % refSyncInterval == 0) {
                    zr = z.get_real().dex_value();
                    zi = z.get_imag().dex_value();
                } else {
                    zr = zr2;
                    zi = zi2;
                    zr.try_normalize();
                    zi.try_normalize();
                }
            }


            if (zr.sgn() == 0) {
                zr = dex_exp::exp10(exp10 * Constants::Fractal::INTENTIONAL_ERROR_REFZERO_POWER);
            }
            if (zi.sgn() == 0) {
                zi = dex_exp::exp10(exp10 * Constants::Fractal::INTENTIONAL_ERROR_REFZERO_POWER);
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
                    const bool sr = zr.sgn() == rr[refIndex].sgn() && zr.sgn() == 0;
                    const bool si = zi.sgn() == ri[refIndex].sgn() && zi.sgn() == 0;

                    if ((sr || std::fabs(static_cast<double>(zr / rr[refIndex]) - 1) <= compressionThreshold) &&
                        (si || std::fabs(static_cast<double>(zi / ri[refIndex]) - 1) <= compressionThreshold) && canReuse) {
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
                std::make_unique<DeepMB2Reference>(std::move(center), std::move(rr), std::move(ri), std::move(tools),
                                                   std::move(periodArray), std::move(*fpgReference), std::move(fpgBn));

        return CreationResult::SUCCESS;
    }


    dex DeepMB2Reference::real(const uint64_t refIteration) const {
        return refReal[ArrayCompressor::compress(compressor, refIteration)];
    }

    dex DeepMB2Reference::imag(const uint64_t refIteration) const {
        return refImag[ArrayCompressor::compress(compressor, refIteration)];
    }


    size_t DeepMB2Reference::length() const { return refReal.size(); }


    uint64_t DeepMB2Reference::longestPeriod() const { return period.back(); }
} // namespace merutilm::rff2
