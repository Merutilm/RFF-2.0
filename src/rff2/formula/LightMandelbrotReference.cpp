//
// Created by Merutilm on 2025-05-09.
//

#include "LightMandelbrotReference.h"

#include <cmath>

#include <cfloat>
#include "../calc/rff_math.h"
#include "../mrthy/ArrayCompressor.h"
#include "../constants/Constants.hpp"

namespace merutilm::rff2 {
    LightMandelbrotReference::LightMandelbrotReference(fp_complex &&center, std::vector<double> &&refReal,
                                                       std::vector<double> &&refImag,
                                                       std::vector<ArrayCompressionTool> &&compressor,
                                                       std::vector<uint64_t> &&period,
                                                       fp_complex &&fpgReference,
                                                       fp_complex &&fpgBn) : MandelbrotReference(std::move(center),
                                                                                 std::move(compressor),
                                                                                 std::move(period),
                                                                                 std::move(fpgReference),
                                                                                 std::move(fpgBn)),
                                                                             refReal(std::move(refReal)),
                                                                             refImag(std::move(refImag)) {
    }

    /**
     * Generates Reference of Mandelbrot set.
     * @param state the processor
     * @param calc calculation settings
     * @param exp10 the exponent of 10 for arbitrary-precision operation
     * @param initialPeriod the initial period. default value is 0. i.e. maximum iterations of arbitrary-precision operation
     * @param dcMax the length of center-to-vertex of screen.
     * @param strictFPG use arbitrary-precision operation for fpg_bn calculation
     * @param actionPerRefCalcIteration the action of every iteration
     * @return the result of generation. but returns @code PROCESS_TERMINATED_REFERENCE@endcode if the process is terminated
     */
    std::unique_ptr<LightMandelbrotReference> LightMandelbrotReference::createReference(
        const ParallelRenderState &state, const FractalAttribute &calc, int exp10, uint64_t initialPeriod,
        double dcMax,
        const bool strictFPG, std::function<void(uint64_t)> &&actionPerRefCalcIteration) {
        if (state.interruptRequested()) {
            return Constants::NullPointer::PROCESS_TERMINATED_REFERENCE;
        }

        auto rr = std::vector<double>();
        auto ri = std::vector<double>();
        rr.push_back(0);
        ri.push_back(0);

        fp_complex center = calc.center;
        auto c = center.edit(exp10);
        auto z = fp_complex_calculator(0, 0, exp10);
        auto fpgBn = fp_complex_calculator(0, 0, exp10);
        auto one = fp_complex_calculator(1.0, 0.0, exp10);
        double bailoutSqr = calc.bailout * calc.bailout;

        double fpgBnr = 1;
        double fpgBni = 0;

        uint64_t iteration = 0;
        double zr = 0;
        double zi = 0;
        uint64_t period = 1;
        auto periodArray = std::vector<uint64_t>();

        auto minZRadius = DBL_MAX;
        uint64_t reuseIndex = 0;

        auto tools = std::vector<ArrayCompressionTool>();
        uint64_t compressed = 0;
        uint64_t maxIteration = calc.maxIteration;
        auto [compressCriteria, compressionThresholdPower, withoutNormalize] = calc.referenceCompAttribute;
        auto func = std::move(actionPerRefCalcIteration);
        double compressionThreshold = compressionThresholdPower <= 0 ? 0 : pow(10, -compressionThresholdPower);
        bool canReuse = withoutNormalize;

        std::unique_ptr<fp_complex> fpgReference = nullptr;

        while (zr * zr + zi * zi < bailoutSqr && iteration < maxIteration) {
            if (iteration % Constants::Fractal::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
                return Constants::NullPointer::PROCESS_TERMINATED_REFERENCE;
            }

            // use Fast-Period-Guessing, and create MPA Table

            if (iteration > 0) {
                double radius2 = zr * zr + zi * zi;

                double fpgLimit = radius2 / dcMax;
                double fpgBnrTemp = fpgBnr * zr * 2 - fpgBni * zi * 2 + 1;
                double fpgBniTemp = fpgBnr * zi * 2 + fpgBni * zr * 2;
                double fpgRadius = rff_math::hypot_approx(fpgBnrTemp, fpgBniTemp);


                if (minZRadius > radius2 && radius2 > 0) {
                    minZRadius = radius2;
                    periodArray.push_back(iteration);
                }

                if (iteration == maxIteration - 1) {
                    periodArray.push_back(iteration);
                    break;
                }

                if ((fpgReference == nullptr && fpgRadius > fpgLimit) || radius2 == 0 || (
                        initialPeriod != 0 && initialPeriod == iteration)) {
                    periodArray.push_back(iteration);
                    fpgReference = std::make_unique<fp_complex>(z);
                    break;
                }

                fpgBnr = fpgBnrTemp;
                fpgBni = fpgBniTemp;
            }


            if (strictFPG) {
                fpgBn *= z.doubled();
                fpgBn += one;
                z.halved();
            }

            //Let's do arbitrary-precision operation!!
            func(iteration);
            z.square();
            z += c;
            zr = z.getReal().double_value();
            zi = z.getImag().double_value();

            uint64_t j = iteration;

            if (!withoutNormalize) {
                for (uint64_t i = periodArray.size(); i > 0; --i) {
                    if (compressCriteria >= periodArray[i - 1]) {
                        break;
                    }
                    j %= periodArray[i - 1];

                    if (j == 0) {
                        canReuse = true;
                        break;
                    }

                    if (j == periodArray[i - 1] - 1) {
                        canReuse = false;
                        break;
                    }
                }
            }


            if (compressCriteria > 0 && iteration >= 1) {
                if (const uint64_t refIndex = ArrayCompressor::compress(tools, reuseIndex + 1);
                    ((zr == rr[refIndex] && zr == 0) || fabs(zr / rr[refIndex] - 1) <= compressionThreshold) &&
                    ((zi == ri[refIndex] && zi == 0) || fabs(zi / ri[refIndex] - 1) <= compressionThreshold) && canReuse
                ) {
                    ++reuseIndex;
                } else if (reuseIndex != 0) {
                    if (reuseIndex > compressCriteria) {
                        // reference compression criteria

                        const auto compressor = ArrayCompressionTool(
                            1, iteration - reuseIndex + 1, iteration);
                        compressed += compressor.range(); //get the increment of iteration
                        tools.push_back(compressor);
                    }
                    //If it is enough to large, set all reference in the range to 0 and save the index

                    reuseIndex = 0;
                    canReuse = withoutNormalize;
                }
            }

            period = ++iteration;

            if (compressCriteria == 0 || reuseIndex <= compressCriteria) {
                if (const uint64_t index = iteration - compressed;
                    index == rr.size()) {
                    rr.push_back(zr);
                    ri.push_back(zi);
                } else {
                    rr[index] = zr;
                    ri[index] = zi;
                }
            }
        }


        if (!strictFPG) fpgBn = fp_complex_calculator(fpgBnr, fpgBni, exp10);
        if (fpgReference == nullptr) fpgReference = std::make_unique<fp_complex>(z);

        rr.resize(period - compressed + 1);
        ri.resize(period - compressed + 1);
        rr.shrink_to_fit();
        ri.shrink_to_fit();
        periodArray = periodArray.empty() ? std::vector(1, period) : periodArray;

        return std::make_unique<LightMandelbrotReference>(std::move(center), std::move(rr), std::move(ri),
                                                          std::move(tools),
                                                          std::move(periodArray), std::move(*fpgReference), fp_complex(fpgBn));
    }


    double LightMandelbrotReference::real(const uint64_t refIteration) const {
        return refReal[ArrayCompressor::compress(compressor, refIteration)];
    }

    double LightMandelbrotReference::imag(const uint64_t refIteration) const {
        return refImag[ArrayCompressor::compress(compressor, refIteration)];
    }


    size_t LightMandelbrotReference::length() const {
        return refReal.size();
    }


    uint64_t LightMandelbrotReference::longestPeriod() const {
        return period.back();
    }
}
