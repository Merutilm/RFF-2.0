//
// Created by Merutilm on 2025-05-09.
//

#include "LightMB2Reference.h"

#include <cmath>

#include <cfloat>
#include <fstream>

#include "../calc/rff_math.h"
#include "../constants/Constants.hpp"
#include "../mrthy/ArrayCompressor.h"
#include "../util/profiler.hpp"

namespace merutilm::rff2 {
    LightMB2Reference::LightMB2Reference(fixed_point_complex_i1 &&center, std::vector<double> &&refReal,
                                                       std::vector<double> &&refImag,
                                                       std::vector<ArrayCompressionTool> &&compressor,
                                                       std::vector<uint64_t> &&period, fixed_point_complex &&fpgReference,
                                                       fixed_point_complex &&fpgBn) :
        MB2Reference(std::move(center), std::move(compressor), std::move(period), std::move(fpgReference),
                            std::move(fpgBn)),
        refReal(std::move(refReal)), refImag(std::move(refImag)) {}

    /**
     * Generates Reference of MB2 set.
     * @param state the processor
     * @param calc calculation settings
     * @param exp10 the exponent of 10 for arbitrary-precision operation
     * @param refInitialCapacity the initial capacity of the reference
     * @param fixedPeriod the fixed period. default value is 0. i.e. maximum iterations of arbitrary-precision
     * operation
     * @param dcMax the length of center-to-vertex of screen.
     * @param strictFPG use arbitrary-precision operation for fpg_bn calculation
     * @param actionPerRefCalcIteration the action of every iteration
     * @param result the result reference.
     * @return the status of this function. returns @code TERMINATED@endcode if the process is
     * terminated
     */
    Reference::CreationResult
    LightMB2Reference::generateReference(const ParallelRenderState &state, const FractalSettings &calc,
                                                int exp10, uint64_t refInitialCapacity, uint64_t fixedPeriod, double dcMax, const bool strictFPG,
                                                std::function<void(uint64_t)> &&actionPerRefCalcIteration,
                                                std::unique_ptr<LightMB2Reference> *result) {
        if (state.interruptRequested()) {
            return CreationResult::TERMINATED;
        }

        auto rr = std::vector<double>();
        auto ri = std::vector<double>();

        rr.reserve(refInitialCapacity);
        ri.reserve(refInitialCapacity);

        rr.push_back(0);
        ri.push_back(0);

        int strictIntExp10 = -exp10;
        int fpgIntExp10 = strictFPG ? strictIntExp10 : 1;

        fixed_point_complex_i1 center = calc.center;
        fixed_point_complex_i1 c = center.create_variant(exp10);
        auto z = fixed_point_complex_i1(0.0, 0.0, exp10);
        auto temp = z;
        auto fpgBn = fixed_point_complex(0.0, 0.0, exp10, fpgIntExp10);
        auto one = fixed_point_complex_i1(1.0, 0.0, exp10);
        double bailoutSqr = calc.bailout * calc.bailout;

        op_thread_pool parallelReferenceThreadPool{};
        bool useParallel = calc.useParallelRefCalculation;

        double fpgBnr = 1;
        double fpgBni = 0;

        double zr = 0;
        double zi = 0;
        double cr = c.get_real().double_value();
        double ci = c.get_imag().double_value();

        auto periodArray = std::vector<uint64_t>();
        auto minZRadius = DBL_MAX;
        uint64_t reuseIndex = 0;

        auto tools = std::vector<ArrayCompressionTool>();
        uint64_t compressed = 0;
        uint64_t maxIteration = calc.maxIteration;

        auto [refSyncInterval, refSyncRadiusPower] = calc.referenceSyncSettings;
        auto [compressCriteria, compressionThresholdPower, withoutNormalize] = calc.referenceCompSettings;

        double compressionThreshold = compressionThresholdPower <= 0 ? 0 : pow(10, -compressionThresholdPower);
        double refSyncRadius2 = pow(10, -refSyncRadiusPower * 2);
        bool canReuse = withoutNormalize;

        std::unique_ptr<fixed_point_complex> fpgReference = nullptr;
        auto func = std::move(actionPerRefCalcIteration);

        uint64_t period;
        for (period = 0; zr * zr + zi * zi < bailoutSqr; ++period) {
            if (state.interruptRequested()) {
                return CreationResult::TERMINATED;
            }

            // use Fast-Period-Guessing to prepare MPA Table creation
            // fpg

            if (period > 0) {

                double radius2 = zr * zr + zi * zi;
                double fpgLimit = radius2 / dcMax;
                double fpgBnrTemp = fpgBnr * zr * 2 - fpgBni * zi * 2 + 1;
                double fpgBniTemp = fpgBnr * zi * 2 + fpgBni * zr * 2;
                double fpgRadius = rff_math::hypot_approx(fpgBnrTemp, fpgBniTemp);


                if (minZRadius > radius2 && radius2 > 0) {
                    minZRadius = radius2;
                    periodArray.push_back(period);
                }

                if ((fpgReference == nullptr && fpgRadius > fpgLimit) || radius2 == 0 ||
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

            // Let's do arbitrary-precision operation!!
            // arbitrary precision

            fixed_point_complex::sqr(z, z, useParallel ? &parallelReferenceThreadPool : nullptr);
            fixed_point_complex::add(z, z, c);

            // double value
            if (refSyncRadiusPower == 0 || refSyncInterval == 1) {
                zr = z.get_real().double_value();
                zi = z.get_imag().double_value();
            } else {
                const double zr2 = (zr + zi) * (zr - zi) + cr;
                const double zi2 = 2 * zr * zi + ci;
                const double radius2 = zr2 * zr2 + zi2 * zi2;

                if (radius2 < refSyncRadius2 || period % refSyncInterval == 0) {
                    zr = z.get_real().double_value();
                    zi = z.get_imag().double_value();
                } else {
                    zr = zr2;
                    zi = zi2;
                }
            }

            // reference compression
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
                    if (((zr == rr[refIndex] && zr == 0) || fabs(zr / rr[refIndex] - 1) <= compressionThreshold) &&
                        ((zi == ri[refIndex] && zi == 0) || fabs(zi / ri[refIndex] - 1) <= compressionThreshold) &&
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

        if (period == 0)
            period = 1; // period is zero when "for" scope was escaped instantly. set the min period to 1.
        if (!strictFPG)
            fpgBn = fixed_point_complex(fpgBnr, fpgBni, exp10, strictIntExp10); // strict fpgBn generated
        if (fpgReference == nullptr)
            fpgReference = std::make_unique<fixed_point_complex>(z);

        rr.resize(period - compressed + 1);
        ri.resize(period - compressed + 1);
        rr.shrink_to_fit();
        ri.shrink_to_fit();
        periodArray = periodArray.empty() ? std::vector(1, period) : periodArray;

        *result = std::make_unique<LightMB2Reference>(std::move(center), std::move(rr), std::move(ri),
                                                             std::move(tools), std::move(periodArray),
                                                             std::move(*fpgReference), std::move(fpgBn));
        return CreationResult::SUCCESS;
    }


    double LightMB2Reference::real(const uint64_t refIteration) const {
        return refReal[ArrayCompressor::compress(compressor, refIteration)];
    }

    double LightMB2Reference::imag(const uint64_t refIteration) const {
        return refImag[ArrayCompressor::compress(compressor, refIteration)];
    }


    size_t LightMB2Reference::length() const { return refReal.size(); }


    uint64_t LightMB2Reference::longestPeriod() const { return period.back(); }
} // namespace merutilm::rff2
