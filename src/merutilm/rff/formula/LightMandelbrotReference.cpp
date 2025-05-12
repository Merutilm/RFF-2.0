//
// Created by Merutilm on 2025-05-09.
//

#include "LightMandelbrotReference.h"

#include <cmath>
#include <complex>
#include <iostream>
#include <optional>

#include "../approx/ApproxMath.h"
#include "../approx/ArrayCompressor.h"
#include "../ui/RFFConstants.h"


LightMandelbrotReference::LightMandelbrotReference(Center &&center, std::vector<double> &&refReal,
                                                   std::vector<double> &&refImag,
                                                   std::vector<ArrayCompressionTool> &&compressor,
                                                   std::vector<uint64_t> &&period,
                                                   Center &&lastReference, Center &&fpgBn) : center(std::move(center)),
    refReal(std::move(refReal)), refImag(std::move(refImag)), compressor(std::move(compressor)),
    period(std::move(period)), lastReference(std::move(lastReference)), fpgBn(std::move(fpgBn)) {
}

/**
 * Generates Reference of Mandelbrot set.
 * @param state the processor
 * @param calc calculation settings
 * @param exp10 the exponent of 10 for arbitrary-precision operation
 * @param initialPeriod the initial period. default value is 0. i.e. maximum iterations of arbitrary-precision operation
 * @param dcMax the length of center-to-vertex of screen.
 * @param strictFPGBn use arbitrary-precision operation for FPG-Bn calculation
 * @param actionPerRefCalcIteration the action of every iteration
 * @return the result of generation. but returns @code nullptr@endcode if the process is terminated
 */
std::unique_ptr<LightMandelbrotReference> LightMandelbrotReference::createReference(
    const ParallelRenderState &state, const CalculationSettings &calc, int exp10, uint64_t initialPeriod, double dcMax,
    bool strictFPGBn, std::function<void(uint64_t)> &&actionPerRefCalcIteration) {
    if (state.interruptRequested()) {
        return nullptr;
    }

    auto rr = std::vector<double>();
    auto ri = std::vector<double>();
    rr.push_back(0);
    ri.push_back(0);

    Center center = calc.center;
    auto c = center.edit(exp10);
    auto z = GMPComplexCalculator(0, 0, exp10);
    auto fpgBn = GMPComplexCalculator(0, 0, exp10);
    auto one = GMPComplexCalculator(1.0, 0.0, exp10);

    double fpgBnr = 0;
    double fpgBni = 0;

    uint64_t iteration = 0;
    double zr = 0;
    double zi = 0;
    uint64_t period = 1;

    auto periodArray = std::vector<uint64_t>();

    auto minZRadius = DBL_MAX;
    int reuseIndex = 0;

    auto tools = std::vector<ArrayCompressionTool>();
    uint64_t compressed = 0;
    double bailoutSquared = calc.bailout * calc.bailout;
    uint64_t maxIteration = calc.maxIteration;
    auto [compressCriteria, compressionThresholdPower] = calc.referenceCompressionSettings;
    auto func = std::move(actionPerRefCalcIteration);

    double compressionThreshold = compressionThresholdPower <= 0 ? 0 : pow(10, -compressionThresholdPower);

    while (zr * zr + zi * zi < bailoutSquared && iteration < maxIteration) {
        if (iteration % RFFConstants::Render::EXIT_CHECK_INTERVAL == 0 && state.interruptRequested()) {
            return nullptr;
        }

        // use Fast-Period-Guessing, and create R3A Table
        double radius2 = zr * zr + zi * zi;
        double fpgLimit = radius2 / dcMax;

        double fpgBnrTemp = fpgBnr * zr * 2 - fpgBni * zi * 2 + 1;
        double fpgBniTemp = fpgBnr * zi * 2 + fpgBni * zr * 2;
        double fpgRadius = ApproxMath::hypotApproximate(fpgBnrTemp, fpgBniTemp);


        if (minZRadius > radius2 && radius2 > 0) {
            minZRadius = radius2;
            periodArray.push_back(iteration);
        }


        if ((iteration >= 1 && fpgRadius > fpgLimit) || iteration == maxIteration - 1 || (
                initialPeriod != 0 && initialPeriod == iteration)) {
            periodArray.push_back(iteration);
            break;
        }

        if (strictFPGBn) {
            fpgBn *= z.doubled();
            fpgBn += one;
        }

        fpgBnr = fpgBnrTemp;
        fpgBni = fpgBniTemp;

        //Let's do arbitrary-precision operation!!
        func(iteration);
        z.square();
        z += c;
        zr = z.getReal().doubleValue();
        zi = z.getImag().doubleValue();

        if (compressCriteria > 0 && iteration >= 1) {
            if (const int64_t refIndex = ArrayCompressor::compress(tools, reuseIndex + 1);
                fabs(zr / rr[refIndex] - 1) <= compressionThreshold &&
                fabs(zi / ri[refIndex] - 1) <= compressionThreshold
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

    if (!strictFPGBn) {
        fpgBn = GMPComplexCalculator(fpgBnr, fpgBni, exp10);
    }

    rr.resize(period - compressed + 1);
    ri.resize(period - compressed + 1);
    rr.shrink_to_fit();
    ri.shrink_to_fit();
    periodArray = periodArray.empty() ? std::vector(1, period) : periodArray;

    return std::make_unique<LightMandelbrotReference>(std::move(center), std::move(rr), std::move(ri), std::move(tools),
                                                      std::move(periodArray), Center(z), Center(fpgBn));
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
